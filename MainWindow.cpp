#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDateTime>
#include <sstream>

// Mock implementation of MenuItem since FoodItem/BeverageItem are maintained by teammates
class MockMenuItem : public MenuItem {
private:
    std::string extraDetails;
    double specialFee;

public:
    MockMenuItem(std::string id, std::string name, std::string category, double price, std::string details = "", double fee = 0.0)
        : MenuItem(id, name, category, price), extraDetails(details), specialFee(fee) {}

    void displayItem() const override {
        // Concrete implementation of pure virtual function
    }

    std::string getExtraDetails() const { return extraDetails; }
    double getSpecialFee() const { return specialFee; }
};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
    populateMenu();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    // Set window properties
    setWindowTitle("Antigravity Restaurant - Billing System");
    resize(1000, 700);

    // Apply premium dark mode styles
    this->setStyleSheet(
        "QMainWindow { background-color: #1a1a24; }"
        "QWidget { color: #e2e8f0; font-family: 'Segoe UI', Arial, sans-serif; font-size: 14px; }"
        "QTableWidget { background-color: #262635; border: 1px solid #3f3f5a; border-radius: 6px; gridline-color: #3f3f5a; }"
        "QTableWidget::item { padding: 5px; }"
        "QHeaderView::section { background-color: #35354a; color: #63b3ed; padding: 6px; font-weight: bold; border: 1px solid #262635; }"
        "QTextEdit { background-color: #262635; border: 1px solid #3f3f5a; border-radius: 6px; color: #a0aec0; font-family: 'Courier New', monospace; font-size: 13px; }"
        "QPushButton { background-color: #3182ce; color: white; border: none; padding: 8px 16px; border-radius: 5px; font-weight: bold; }"
        "QPushButton:hover { background-color: #4299e1; }"
        "QPushButton:pressed { background-color: #2b6cb0; }"
        "QPushButton#clearBtn { background-color: #e53e3e; }"
        "QPushButton#clearBtn:hover { background-color: #fc8181; }"
        "QPushButton#clearBtn:pressed { background-color: #c53030; }"
        "QSpinBox, QDoubleSpinBox { background-color: #262635; border: 1px solid #3f3f5a; border-radius: 5px; padding: 4px; color: white; }"
        "QLabel { font-weight: 500; }"
        "QLabel#titleLabel { font-size: 24px; font-weight: bold; color: #63b3ed; margin-bottom: 10px; }"
        "QLabel#totalLabel { font-size: 20px; font-weight: bold; color: #48bb78; }"
    );

    // Main layout structures
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralWidget");
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Title
    QLabel *titleLabel = new QLabel("Antigravity Restaurant Billing", this);
    titleLabel->setObjectName("titleLabel");
    mainLayout->addWidget(titleLabel);

    // Horizonal split: Left is Menu & Order, Right is Receipt
    QHBoxLayout *splitLayout = new QHBoxLayout();
    mainLayout->addLayout(splitLayout);

    // Left pane
    QVBoxLayout *leftPane = new QVBoxLayout();
    splitLayout->addLayout(leftPane, 3); // 3/5 ratio

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
    // Add mock items using the MockMenuItem class
    menuItems.push_back(std::make_shared<MockMenuItem>("F001", "Margherita Pizza", "Main Course", 12.99, "Italian | Prep: 12m | Spicy: No", 6.00));
    menuItems.push_back(std::make_shared<MockMenuItem>("F002", "Spicy Chicken Burger", "Main Course", 8.99, "American | Prep: 8m | Spicy: Yes", 4.00));
    menuItems.push_back(std::make_shared<MockMenuItem>("F003", "Bruschetta", "Appetizer", 6.50, "Italian | Prep: 5m | Spicy: No", 2.50));
    menuItems.push_back(std::make_shared<MockMenuItem>("F004", "Chocolate Lava Cake", "Dessert", 7.25, "French | Prep: 10m | Spicy: No", 5.00));
    menuItems.push_back(std::make_shared<MockMenuItem>("B001", "Iced Latte", "Beverage", 4.50, "350ml | Coffee | Carb: No"));
    menuItems.push_back(std::make_shared<MockMenuItem>("B002", "Coca Cola", "Beverage", 2.50, "330ml | Soft Drink | Carb: Yes"));
    menuItems.push_back(std::make_shared<MockMenuItem>("B003", "Fresh Orange Juice", "Beverage", 5.00, "400ml | Juice | Carb: No"));

    menuTable->setRowCount(menuItems.size());
    for (size_t i = 0; i < menuItems.size(); ++i) {
        auto item = menuItems[i];
        menuTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(item->getItemID())));
        menuTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(item->getName())));
        menuTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(item->getCategory())));
        menuTable->setItem(i, 3, new QTableWidgetItem(QString("$%1").arg(item->getPrice(), 0, 'f', 2)));
        
        QString details = "";
        if (auto mockItem = std::dynamic_pointer_cast<MockMenuItem>(item)) {
            details = QString::fromStdString(mockItem->getExtraDetails());
        }
        menuTable->setItem(i, 4, new QTableWidgetItem(details));
    }
}

void MainWindow::onAddItemClicked() {
    int selectedRow = menuTable->currentRow();
    if (selectedRow < 0) {
        QMessageBox::warning(this, "No Selection", "Please select a menu item from the list.");
        return;
    }

    auto selectedItem = menuItems[selectedRow];
    int qty = quantitySpinner->value();

    // Check if already in order
    bool found = false;
    for (auto &orderItem : currentOrder) {
        if (orderItem.first->getItemID() == selectedItem->getItemID()) {
            orderItem.second += qty;
            found = true;
            break;
        }
    }

    if (!found) {
        currentOrder.push_back({selectedItem, qty});
    }

    updateOrderTable();
    quantitySpinner->setValue(1); // reset spinner
}

void MainWindow::onRemoveItemClicked() {
    int selectedRow = orderTable->currentRow();
    if (selectedRow < 0) {
        QMessageBox::warning(this, "No Selection", "Please select an item from the order list to remove.");
        return;
    }

    currentOrder.erase(currentOrder.begin() + selectedRow);
    updateOrderTable();
}

void MainWindow::updateOrderTable() {
    orderTable->setRowCount(currentOrder.size());
    double tempSubtotal = 0.0;

    for (size_t i = 0; i < currentOrder.size(); ++i) {
        auto item = currentOrder[i].first;
        int qty = currentOrder[i].second;
        double subtotal = item->getPrice() * qty;
        tempSubtotal += subtotal;

        orderTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(item->getName())));
        orderTable->setItem(i, 1, new QTableWidgetItem(QString("$%1").arg(item->getPrice(), 0, 'f', 2)));
        orderTable->setItem(i, 2, new QTableWidgetItem(QString::number(qty)));
        orderTable->setItem(i, 3, new QTableWidgetItem(QString("$%1").arg(subtotal, 0, 'f', 2)));
    }

    totalLabel->setText(QString("Subtotal: $%1").arg(tempSubtotal, 0, 'f', 2));
}

void MainWindow::onCalculateBillClicked() {
    if (currentOrder.empty()) {
        QMessageBox::warning(this, "Empty Order", "No items added to the order yet.");
        return;
    }

    double subtotal = 0.0;
    double totalPrepFee = 0.0;

    std::stringstream ss;
    ss << "========================================\n";
    ss << "         ANTIGRAVITY RESTAURANT         \n";
    ss << "            INVOICE / RECEIPT           \n";
    ss << "========================================\n";
    ss << "Date/Time: " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString() << "\n";
    ss << "----------------------------------------\n";
    ss << std::left << std::setw(20) << "Item Name" 
       << std::setw(8) << "Price" 
       << std::setw(5) << "Qty" 
       << "Subtotal\n";
    ss << "----------------------------------------\n";

    for (const auto &pair : currentOrder) {
        auto item = pair.first;
        int qty = pair.second;
        double price = item->getPrice();
        double itemSubtotal = price * qty;
        subtotal += itemSubtotal;

        ss << std::left << std::setw(20) << (item->getName().length() > 18 ? item->getName().substr(0, 17) + "..." : item->getName())
           << "$" << std::setw(7) << std::fixed << std::setprecision(2) << price
           << std::setw(5) << qty
           << "$" << itemSubtotal << "\n";

        // Add special fees/calculations if applicable
        if (auto mockItem = std::dynamic_pointer_cast<MockMenuItem>(item)) {
            double prepFee = mockItem->getSpecialFee() * qty;
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
    currentOrder.clear();
    updateOrderTable();
    receiptDisplay->clear();
    totalLabel->setText("Total: $0.00");
    discountSpinner->setValue(0.0);
}

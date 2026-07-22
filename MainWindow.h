#pragma once

#include <QMainWindow>
#include <QTableWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTextEdit>
#include <vector>
#include <memory>
#include "MenuItem.h"

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

private:
    void setupUI();
    void populateMenu();
    void updateOrderTable();

    // Data members
    std::vector<std::shared_ptr<MenuItem>> menuItems;
    std::vector<std::pair<std::shared_ptr<MenuItem>, int>> currentOrder; // (Item, Quantity)

    // UI Widgets
    QTableWidget *menuTable;
    QTableWidget *orderTable;
    QSpinBox *quantitySpinner;
    QDoubleSpinBox *discountSpinner;
    QDoubleSpinBox *taxSpinner;
    QPushButton *addButton;
    QPushButton *removeButton;
    QPushButton *calculateButton;
    QPushButton *clearButton;
    QTextEdit *receiptDisplay;
    QLabel *totalLabel;
};

#include "Example4.h"
#include "ui_Example4.h"

Example4::Example4(QWidget *parent) : QWidget(parent), ui(new Ui::Example4) {
    ui->setupUi(this);
}

Example4::~Example4() {
    delete ui;
}

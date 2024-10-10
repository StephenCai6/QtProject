#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <math.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // this->setStyleSheet("QPushButton{background-color: red}");

    digitBTNs = {{Qt::Key_0,ui->btnNum0},
               {Qt::Key_1,ui->btnNum1},
               {Qt::Key_2,ui->btnNum2},
               {Qt::Key_3,ui->btnNum3},
               {Qt::Key_4,ui->btnNum4},
               {Qt::Key_5,ui->btnNum5},
               {Qt::Key_6,ui->btnNum6},
               {Qt::Key_7,ui->btnNum7},
               {Qt::Key_8,ui->btnNum8},
               {Qt::Key_9,ui->btnNum9},
               };

    foreach(auto btn,digitBTNs)
        connect(btn,SIGNAL(clicked()),this,SLOT(btnNumClicked()));

    // connect(ui->btnNum0,SIGNAL(clicked()),this,SLOT(btnNumClicked()));
    // connect(ui->btnNum1,SIGNAL(clicked()),this,SLOT(btnNumClicked()));
    // connect(ui->btnNum2,SIGNAL(clicked()),this,SLOT(btnNumClicked()));
    // connect(ui->btnNum3,SIGNAL(clicked()),this,SLOT(btnNumClicked()));
    // connect(ui->btnNum4,SIGNAL(clicked()),this,SLOT(btnNumClicked()));
    // connect(ui->btnNum5,SIGNAL(clicked()),this,SLOT(btnNumClicked()));
    // connect(ui->btnNum6,SIGNAL(clicked()),this,SLOT(btnNumClicked()));
    // connect(ui->btnNum7,SIGNAL(clicked()),this,SLOT(btnNumClicked()));
    // connect(ui->btnNum8,SIGNAL(clicked()),this,SLOT(btnNumClicked()));
    // connect(ui->btnNum9,SIGNAL(clicked()),this,SLOT(btnNumClicked()));

    connect(ui->btnPlus,SIGNAL(clicked()),this,SLOT(btnBinaryOperatorClicked()));
    connect(ui->btnMinus,SIGNAL(clicked()),this,SLOT(btnBinaryOperatorClicked()));
    connect(ui->btnMultiple,SIGNAL(clicked()),this,SLOT(btnBinaryOperatorClicked()));
    connect(ui->btnDivide,SIGNAL(clicked()),this,SLOT(btnBinaryOperatorClicked()));

    connect(ui->btnPercnetge,SIGNAL(clicked()),this,SLOT(btnUnaryOperatorClicked()));
    connect(ui->btnInverse,SIGNAL(clicked()),this,SLOT(btnUnaryOperatorClicked()));
    connect(ui->btnSquare,SIGNAL(clicked()),this,SLOT(btnUnaryOperatorClicked()));
    connect(ui->btnSqrt,SIGNAL(clicked()),this,SLOT(btnUnaryOperatorClicked()));
    connect(ui->btnSign,SIGNAL(clicked()),this,SLOT(btnUnaryOperatorClicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 数字0~9
void MainWindow::btnNumClicked()
{
    QString digit = qobject_cast<QPushButton*>(sender())->text();

    if (digit == "0" && operand == "0")
        digit = "";

    if(operand == "0" && digit != "0" && digit != "")
        operand = "";

    operand += digit;

    ui->display->setText(operand);

    // 如果之前有等号结果，点击操作数后清空
    if (!equalResult.isNull()) {
        equalResult.clear();
    }
}

// 运算符+-×÷
void MainWindow::btnBinaryOperatorClicked()
{
    QString opcode = qobject_cast<QPushButton*>(sender())->text();
    qDebug() << opcode;

    if (operand != "") {
        operands.push_back(operand);
        operand = "";
    }

    // 如果之前点击过等号，且当前operand为空，则使用之前的结果作为新的操作数
    if (!equalResult.isNull()) {
        operands.push_back(equalResult);
        equalResult.clear(); // 清空之前的结果
    }
    opcodes.push_back(opcode);

    // 确保有足够的操作数来执行操作
    if (operands.size() < 1) {
        // 不足以执行操作，可以在这里提醒用户
        ui->statusbar->showMessage("Please enter at least one operand before clicking an operator.");
        return; // 退出函数
    }

    QString result = calculation();
    ui->display->setText(result);
}

// 双操作数操作符处理
QString MainWindow::calculation(bool *ok)
{
    double result = operands.front().toDouble();
    if(operands.size()==2 && opcodes.size() > 0){
        // 取操作数
        double operand1 = operands.front().toDouble();
        operands.pop_front();
        double operand2 = operands.front().toDouble();
        operands.pop_front();

        // 取操作符
        QString op = opcodes.front();
        opcodes.pop_front();

        // 构建表达式
        QString expression = QString::number(operand1) + " " + op +
                " " + QString::number(operand2) + " = ";

        if(op == "+"){
            result = operand1 + operand2;
        }else if(op == "-"){
            result = operand1 - operand2;
        }else if(op == "×"){
            result = operand1 * operand2;
        }else if(op == "÷"){
            if(operand2 != 0) {
                result = operand1 / operand2;
            } else {
                // 如果除数为0，显示错误信息
                ui->statusbar->showMessage("Error: Division by zero");
                return "被除数不能为0";
            }
        }

        // 将结果显示在表达式之后
        expression += QString::number(result);
        ui->statusbar->showMessage(expression);

        operands.push_back(QString::number(result));
    } else
        ui->statusbar->showMessage(QString("operands is %1,opcodes is %2")
                    .arg(operands.size()).arg(opcodes.size()));

    return QString::number(result);
}

// 单操作数操作符处理
void MainWindow::btnUnaryOperatorClicked()
{
    if (operand != "" || !equalResult.isNull()) {
        double result;
        if (!equalResult.isNull()){
            result = equalResult.toDouble();
            equalResult.clear();
        }else {
            result = operand.toDouble();
            operand.clear(); // 清空当前操作数，准备下一次输入
        }
        // operand = "";

        QString op = qobject_cast<QPushButton*>(sender())->text();
        if(op == "%")
            result /= 100.0;
        else if(op == "1/x ")
            result = 1/result;
        else if(op == "x^2")
            result *= result;
        else if(op == "√x")
            result = sqrt(result);
        else if(op == "±")
            result = - result;

        operand = QString::number(result);
        ui->display->setText(operand);
    }
}

// 小数点.
void MainWindow::on_btnPeriod_clicked()
{
    if(operand == "")
        operand = "0.";
    if(!operand.contains("."))
        operand += qobject_cast<QPushButton*>(sender())->text();
    ui->display->setText(operand);
}

// 退格⌫
void MainWindow::on_btnDel_clicked()
{
    if (equalResult.isNull()) {
        operand = operand.left(operand.length() - 1);
        ui->display->setText(operand);
    }
}

// 清除当前所有的输入C
void MainWindow::on_btnClear_clicked()
{
    operand.clear();
    operands.clear();
    opcode.clear();
    opcodes.clear();
    ui->statusbar->showMessage("");
    ui->display->setText(operand);
}

// 清除最近的输入CE
void MainWindow::on_btnClearEntry_clicked()
{
    if (equalResult.isNull()) {
        operand.clear();
        ui->display->setText(operand);
    }
}

// 等于=
void MainWindow::on_btnEqual_clicked()
{
    if (operand != "") {
        operands.push_back(operand);
        operand = "";
    }
    if (operands.size() >= 2 && !opcodes.isEmpty()) {
        equalResult = calculation();
        ui->display->setText(equalResult);

        operands.clear();
        opcodes.clear();
    }else {
        ui->statusbar->showMessage("Error: Not enough operands or operators for calculation.");
    }
}

// 键盘事件处理
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    foreach (auto btnKey,digitBTNs.keys()) {
        if (event->key() == btnKey)
            digitBTNs[btnKey]->animateClick();
    }
    if (event->key() == Qt::Key_Plus)
        ui->btnPlus->animateClick();
    else if (event->key() == Qt::Key_Minus)
        ui->btnMinus->animateClick();
    else if (event->key() == Qt::Key_Asterisk)
        ui->btnMultiple->animateClick();
    else if (event->key() == Qt::Key_Slash)
        ui->btnDivide->animateClick();
    else if (event->key() == Qt::Key_Equal)
        ui->btnEqual->animateClick();
    else if (event->key() == Qt::Key_C)
        ui->btnClear->animateClick();
    else if (event->key() == Qt::Key_Backspace)
        ui->btnDel->animateClick();
    else if (event->key() == Qt::Key_Period)
        ui->btnPeriod->animateClick();
    else if (event->key() == Qt::Key_Percent)
        ui->btnPercnetge->animateClick();
}



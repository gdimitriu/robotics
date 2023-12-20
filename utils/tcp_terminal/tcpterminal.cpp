#include "tcpterminal.h"
#include "ui_tcpterminal.h"

TcpTerminal::TcpTerminal(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TcpTerminal)
{
    ui->setupUi(this);
}

TcpTerminal::~TcpTerminal()
{
    delete ui;
}

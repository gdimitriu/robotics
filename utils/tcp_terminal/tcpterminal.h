#ifndef TCPTERMINAL_H
#define TCPTERMINAL_H

#include <QMainWindow>

namespace Ui {
class TcpTerminal;
}

class TcpTerminal : public QMainWindow
{
    Q_OBJECT

public:
    explicit TcpTerminal(QWidget *parent = 0);
    ~TcpTerminal();

private:
    Ui::TcpTerminal *ui;
};

#endif // TCPTERMINAL_H

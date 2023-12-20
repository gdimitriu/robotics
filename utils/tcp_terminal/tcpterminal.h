#ifndef TCPTERMINAL_H
#define TCPTERMINAL_H

#include <QMainWindow>
#include <QTcpSocket>
namespace Ui {
class TcpTerminal;
}

class TcpTerminal : public QMainWindow
{
    Q_OBJECT
private slots:
    void connectTo();
    void disconnectFrom();
    void sendData();
    void sockConnected();
    void sockDisconnected();
public:
    explicit TcpTerminal(QWidget *parent = 0);
    ~TcpTerminal();

private:
    void init();

    QTcpSocket *socket;

    Ui::TcpTerminal *ui;
};

#endif // TCPTERMINAL_H

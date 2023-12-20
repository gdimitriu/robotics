#include "tcpterminal.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TcpTerminal w;
    w.show();

    return a.exec();
}

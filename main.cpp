#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setFixedSize(800, 600);
    w.setWindowTitle("Wana Decrypt0r 2.0");
    w.setWindowIcon(QIcon(":/icon/handshake.jpg"));
    w.show();
    return a.exec();
}

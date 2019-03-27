#include "mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    //ustawienie tytulu
    w.setWindowTitle("Program do optymalizacji zadania PLC metodą odcięć");

    w.show();

    return a.exec();
}

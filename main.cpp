#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.setApplicationName("Let's Study Smart");
    a.setApplicationVersion("2.0");
    MainWindow w;
    w.show();
    return a.exec();
}

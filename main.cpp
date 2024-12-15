#include "include/mainwindow.h"
#include <QApplication>
#include <QFile>


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QFile styleFile("./style/Diffnes.qss");
    styleFile.open(QFile::ReadOnly);
    QString styleString = QLatin1String(styleFile.readAll());
    a.setStyleSheet(styleString);

    mainwindow w;
    w.resize(800,600);
    w.show();
    return a.exec();
}

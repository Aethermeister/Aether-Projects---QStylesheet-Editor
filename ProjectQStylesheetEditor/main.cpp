#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/Resources/style_colored_24dp.png"));

    QCoreApplication::setApplicationName("QStylesheet Editor");
    QCoreApplication::setApplicationVersion("1.0");
    QCoreApplication::setOrganizationName("Aether Projects");

    MainWindow w;
    w.show();
    return a.exec();
}

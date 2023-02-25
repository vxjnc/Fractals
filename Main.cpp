#include "MainWindow/MainWindow.h"
#include <QApplication>
#include <QLocale>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    qApp->setStyle("plastique");
    qApp->setStyle("cleanlooks");
    qApp->setStyle("fusion");
    qApp->setStyle("gtk2");

    std::setlocale(LC_ALL, "C");
    QLocale::setDefault(QLocale::c());

    MainWindow window;
    window.show();

    app.exec();

    return 0;
}

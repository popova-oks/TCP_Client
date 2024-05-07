
#include <QApplication>
#include "application.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Application appl;
    appl.show();

    return a.exec();
}

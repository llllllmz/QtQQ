#include "UserLogin.h"
#include <QtWidgets/QApplication>
#include "UseMysql.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);       //���һ������رգ����˳�����
    UserLogin w;
    w.show();

    return a.exec();
}

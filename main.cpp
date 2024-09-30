#include "UserLogin.h"
#include <QtWidgets/QApplication>
#include "UseMysql.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);       //最后一个窗体关闭，不退出程序
    UserLogin w;
    w.show();

    return a.exec();
}

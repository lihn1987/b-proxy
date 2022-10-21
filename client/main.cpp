#include "widget_main.h"

#include <QApplication>
#include <stdio.h>
#include <QDebug>
int main(int argc, char *argv[])
{
    setbuf(stdout, 0);
    setbuf(stderr, 0);
    setvbuf(stdout,NULL,_IONBF,0);
    printf("123456");
    qDebug()<<"???";
    QApplication a(argc, argv);
    WidgetMain w;
    w.show();
    printf("123456");
    fflush(stdout);
    return a.exec();
}

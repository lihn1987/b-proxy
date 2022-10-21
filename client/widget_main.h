#ifndef WIDGETMAIN_H
#define WIDGETMAIN_H

#include <QWidget>
#include "proxy.h"
QT_BEGIN_NAMESPACE
namespace Ui { class WidgetMain; }
QT_END_NAMESPACE
class AsioManager;
class ClientServer;
class WidgetMain : public QWidget
{
    Q_OBJECT

public:
    WidgetMain(QWidget *parent = nullptr);
    ~WidgetMain();
private:
    void InitUI();
private slots:
    void on_btn_main_connect_clicked(bool checked);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::WidgetMain *ui;
    Proxy proxy;
    std::shared_ptr<AsioManager> ios_manager;
    std::shared_ptr<ClientServer> server;
};
#endif // WIDGETMAIN_H

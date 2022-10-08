#ifndef WIDGETMAIN_H
#define WIDGETMAIN_H

#include <QWidget>
#include "proxy.h"
QT_BEGIN_NAMESPACE
namespace Ui { class WidgetMain; }
QT_END_NAMESPACE

class WidgetMain : public QWidget
{
    Q_OBJECT

public:
    WidgetMain(QWidget *parent = nullptr);
    ~WidgetMain();
private:
    void InitUI();
    void OnV2RayLog(const std::string& log);
    void OnCommonLog(const std::string& log);
signals:
    void sig_v2ray_log(const std::string& log);
    void sig_common_log(const std::string& log);
private slots:
    void on_btn_main_connect_clicked(bool checked);
    void DoV2RayLog(const std::string& log);
    void DoCommonLog(const std::string& log);
    void on_btn_export_account_clicked();

private:
    Ui::WidgetMain *ui;
    Proxy proxy;
};
#endif // WIDGETMAIN_H

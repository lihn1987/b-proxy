#include "widget_main.h"
#include "ui_widget_main.h"
#include <QDebug>
WidgetMain::WidgetMain(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WidgetMain){
    ui->setupUi(this);
    this->InitUI();
    proxy.AddCommonLogCallBack(boost::bind(&WidgetMain::OnCommonLog, this, boost::placeholders::_1));
    proxy.AddV2RayLogCallBack(boost::bind(&WidgetMain::OnV2RayLog, this, boost::placeholders::_1));
    connect(this, &WidgetMain::sig_v2ray_log, this, &WidgetMain::DoV2RayLog);
    connect(this, &WidgetMain::sig_common_log, this, &WidgetMain::DoCommonLog);
}

WidgetMain::~WidgetMain(){
    delete ui;
}

void WidgetMain::InitUI(){
    ui->btn_main_connect->setText(QString::fromWCharArray(L"点击连接"));
}

void WidgetMain::OnV2RayLog(const std::string &log){
    emit sig_v2ray_log(log);
}

void WidgetMain::OnCommonLog(const std::string &log){
    emit sig_common_log(log);
}


void WidgetMain::on_btn_main_connect_clicked(bool checked){
    if (checked == true) {
        ui->btn_main_connect->setText(QString::fromWCharArray(L"点击断开"));
        proxy.Connect();
    } else {
        ui->btn_main_connect->setText(QString::fromWCharArray(L"点击连接"));
        proxy.Disconnect();
    }
}

void WidgetMain::DoV2RayLog(const std::string &log){
//    ui->txt_log_v2ray->append(QString::fromStdString(log));
    QTextCursor tc = ui->txt_log_v2ray->textCursor();
    tc.movePosition(QTextCursor::End);
    tc.insertText(QString::fromStdString(log));
}

void WidgetMain::DoCommonLog(const std::string &log)
{

}


void WidgetMain::on_btn_export_account_clicked()
{


}


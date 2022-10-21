#include "widget_main.h"
#include "ui_widget_main.h"
#include <QDebug>
#include "../asio_manager/asio_manager.h"
#include "../net_lib/client_server.h"
#include "../crypto_tools/key_tools.h"
#include "../log/log.h"
WidgetMain::WidgetMain(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WidgetMain){
    ui->setupUi(this);
    this->InitUI();
    ios_manager = GetAsioManager();
    server = std::make_shared<ClientServer>();
}

WidgetMain::~WidgetMain(){
    delete ui;
}

void WidgetMain::InitUI(){
    ui->btn_main_connect->setText(QString::fromWCharArray(L"点击连接"));
}


void WidgetMain::on_btn_main_connect_clicked(bool checked){
    if (checked == true) {
        ui->btn_main_connect->setText(QString::fromWCharArray(L"点击断开"));
        ProxyLocalConfig cfg;
        cfg.enable = 1;
        cfg.server = "127.0.0.1:1011";
        // proxy.SetProxyLocalConfig(cfg);

        ios_manager->StartThreads();
        server->StartServer(ios_manager->GetIoService());
    } else {
        ui->btn_main_connect->setText(QString::fromWCharArray(L"点击连接"));
        ProxyLocalConfig cfg;
        cfg.enable = 0;
        cfg.server = "127.0.0.1:1011";
        proxy.SetProxyLocalConfig(cfg);

        server->StopServer();
        ios_manager->StopThreads();
    }
}



void WidgetMain::on_pushButton_clicked()
{
//    client->StartWithConnect({boost::asio::ip::make_address("127.0.0.1"), 9922}, "/test");
    std::string key = "12345678901234567890123456789012";
    std::string iv = "1234567890123456";
    std::string ddd = "ssk";
    std::string out1, out2;
    bool test = AesEncode(key, iv, ddd, out1);
    GetProxyLog()->LogDebug(out1);
    test = false;
    test = AesDecode("12345678901234567890123456789012", "1234567890123456", out1, out2);
    GetProxyLog()->LogDebug(out2);
    test = false;
}


void WidgetMain::on_pushButton_2_clicked()
{
//    auto for_send = ui->plainTextEdit->toPlainText().toUtf8();
//    auto for_send_str = std::string(for_send.data(),for_send.length());
//    client->Write(for_send_str);
}


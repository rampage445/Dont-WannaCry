#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPalette>
#include <QPixmap>
#include <QTimer>
#include <QMessageBox>
#include <QClipboard>
#include <windows.h>
#include <wincrypt.h>
#include <cstdio>
#include <vector>
#include <string>
#include <stack>
#include <iostream>

#pragma comment(lib, "crypt32.lib")

using namespace std;


void write_file(const string& path) {
    FILE* f = fopen(path.c_str(), "wb");
    if (!f) return;
    const char* text = "This is a dummy file.";
    fwrite(text, 1, strlen(text), f);
    fclose(f);
}

void encrypt_file(const string& path) {
    FILE* f = fopen(path.c_str(), "rb");
    if (!f) return;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size <= 0) {
        fclose(f);
        return;
    }

    vector<BYTE> data(size);
    fread(data.data(), 1, size, f);
    fclose(f);

    DATA_BLOB input = { (DWORD)data.size(), data.data() };
    DATA_BLOB output;

    if (!CryptProtectData(&input, nullptr, nullptr, nullptr, nullptr, 0, &output)) return;

    f = fopen(path.c_str(), "wb");
    if (!f) {
        LocalFree(output.pbData);
        return;
    }

    fwrite(output.pbData, 1, output.cbData, f);
    fclose(f);
    LocalFree(output.pbData);
}

void decrypt_file(const string& path) {
    FILE* f = fopen(path.c_str(), "rb");
    if (!f) return;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size <= 0) {
        fclose(f);
        return;
    }

    vector<BYTE> encrypted(size);
    fread(encrypted.data(), 1, size, f);
    fclose(f);

    DATA_BLOB input = { (DWORD)size, encrypted.data() };
    DATA_BLOB output;

    if (!CryptUnprotectData(&input, nullptr, nullptr, nullptr, nullptr, 0, &output)) return;

    f = fopen(path.c_str(), "wb");
    if (!f) {
        LocalFree(output.pbData);
        return;
    }

    fwrite(output.pbData, 1, output.cbData, f);
    fclose(f);
    LocalFree(output.pbData);
}

void create_and_encrypt() {
    string file = "wannacry.txt";
    write_file(file);
    encrypt_file(file);
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QPalette pal = this->palette();
    pal.setColor(QPalette::Window, QColor("#8A0303"));
    this->setAutoFillBackground(true);
    this->setPalette(pal);


    QPixmap pixmap(":/icon/lock.jpg");
    ui->lock->setPixmap(pixmap);
    ui->lock->setFixedSize(160,160);
    ui->lock->setScaledContents(true);

    ui->timewidget->setStyleSheet("#timewidget { border: 1px solid white; }");
    ui->timewidget2->setStyleSheet("#timewidget2 { border: 1px solid white; }");

    ui->time1->setText("Payment will be raised on");
    ui->time1->setStyleSheet("color: yellow;");
    ui->date1->setText("08/24/25 00:47:89");
    ui->left1->setText("Time Left");
    ui->timestamp1->setText("02:19:47:78");

    ui->time2->setText("Payment will be raised on");
    ui->time2->setStyleSheet("color: yellow;");
    ui->date2->setText("08/28/25 00:47:89");
    ui->left2->setText("Time Left");
    ui->timestamp2->setText("06:19:47:78");


    auto style_label = [](auto label) {
        QFont font = label->font();
        font.setBold(true);
        label->setFont(font);
        label->setStyleSheet("color: white;");
        label->setAlignment(Qt::AlignCenter);
    };

    auto style_big = [](auto label,int size) {
        QFont font = label->font();
        font.setBold(true);
        font.setPointSize(size);
        label->setFont(font);
        label->setStyleSheet("color: white;");
        label->setAlignment(Qt::AlignCenter);
    };
    style_big(ui->oops,15);
    style_label(ui->date1);
    style_label(ui->left1);
    style_big(ui->timestamp1,20);

    style_label(ui->date2);
    style_label(ui->left2);
    style_big(ui->timestamp2,20);


    // ComboBox
    ui->comboBox->addItem("English");
    ui->comboBox->addItem("Bangla");

    ui->comboBox->setFixedWidth(100);


    // About
    auto underline_skyblue = [](auto item, QString text) {
        item->setText(text);
        QFont font = item->font();
        font.setUnderline(true);
        item->setFont(font);
        item->setStyleSheet("color: skyblue;");
    };

    underline_skyblue(ui->about, "About Bitcoin");
    underline_skyblue(ui->buybit, "How to buy bitcoins?");
    underline_skyblue(ui->contact, "Contact Us");

    // Ransom text
    QString ransomTextEng = R"(
    <div style="font-size:14pt; font-family: Arial, sans-serif;">
      <h2>DON'T WANNACRY RANSOMWARE</h2>

      <p>Oh no! Your files have been kidnapped by the notorious DontWannaCry gang.</p>

      <h2>What happened?</h2>

      <p>We just wanted to have some fun, but your files decided to go on a vacation without telling you.</p>

      <h2>If you don’t wanna cry...</h2>

      <p>Send us $300 worth of Bitcoin to the following wallet:<br>
      <b>1taBitcoinODiboNaTumake</b></p>

      <p><h2>Why pay?</h2> If you want your files back you have to pay.</p>

      <p>Stay safe and keep laughing!</p>
    </div>
    )";

    QString ransomTextBangla = R"(
  <div style="font-size:14pt; font-family: Arial, sans-serif;">
    <h2>ডন'ট ওয়ানাক্রাই র‍্যানসমওয়্যার</h2>

    <p>আহা! আপনার ফাইলগুলি কুখ্যাত ডন'ট ওয়ানাক্রাই গ্যাং দ্বারা অপহৃত হয়েছে।</p>

    <h2>কি হয়েছে?</h2>

    <p>আমরা শুধু একটু মজা করতে চেয়েছিলাম, কিন্তু আপনার ফাইলগুলি আপনাকে না জানিয়ে ছুটি নেয়ার সিদ্ধান্ত নিয়েছিল।</p>

    <h2>যদি আপনি কাঁদতে না চান...</h2>

    <p>আমাদেরকে নিম্নলিখিত ওয়ালেটে $৩০০ বিটকয়েন পাঠান:<br>
    <b>1taBitcoinODiboNaTumake</b></p>

    <p><h2>কেন দিতে হবে?</h2> যদি আপনি আপনার ফাইল ফিরে পেতে চান, তাহলে দিতে হবে।</p>

    <p>সতর্ক থাকুন এবং হাসি বজায় রাখুন!</p>
  </div>
)";

    ui->textEdit->setReadOnly(true);
    ui->textEdit->setHtml(ransomTextEng);

    //bitcoin address
    QPixmap pixmap2(":/icon/bitcoin.png");
    ui->bitcoinimage->setPixmap(pixmap2);

    ui->send->setText("Send $300 worth of bitcoin to this address");
    QFont font = ui->send->font();
    font.setBold(true);
    font.setPointSize(12);
    ui->send->setFont(font);
    ui->send->setStyleSheet("color: yellow;");

    ui->btcaddr->setText("1taBitcoinODiboNaTumake");
    QFont font2 = ui->btcaddr->font();
    font2.setBold(true);
    font2.setPointSize(10);
    ui->btcaddr->setFont(font2);
    ui->btcaddr->setStyleSheet("#btcaddr { border: 1px solid white; color: white; }");

    // Dropdown
    connect(ui->comboBox, &QComboBox::currentTextChanged, this,
            [this, ransomTextEng, ransomTextBangla](const QString &text){
                if (text == "English")
                    ui->textEdit->setHtml(ransomTextEng);
                else if (text == "Bangla")
                    ui->textEdit->setHtml(ransomTextBangla);
            });

    // Timer

    timer = new QTimer(this);

    const int fourDaysInSeconds = 4 * 24 * 3600;

    connect(timer, &QTimer::timeout, this, [this, fourDaysInSeconds]() {
        if (totalSeconds > 0) {
            totalSeconds--;

            int days1 = totalSeconds / (24*3600);
            int hours1 = (totalSeconds % (24*3600)) / 3600;
            int minutes1 = (totalSeconds % 3600) / 60;
            int seconds1 = totalSeconds % 60;

            int totalSeconds2 = totalSeconds + fourDaysInSeconds;
            int days2 = totalSeconds2 / (24*3600);
            int hours2 = (totalSeconds2 % (24*3600)) / 3600;
            int minutes2 = (totalSeconds2 % 3600) / 60;
            int seconds2 = totalSeconds2 % 60;

            QString timeString1 = QString("%1:%2:%3:%4")
                                      .arg(days1, 2, 10, QChar('0'))
                                      .arg(hours1, 2, 10, QChar('0'))
                                      .arg(minutes1, 2, 10, QChar('0'))
                                      .arg(seconds1, 2, 10, QChar('0'));

            QString timeString2 = QString("%1:%2:%3:%4")
                                      .arg(days2, 2, 10, QChar('0'))
                                      .arg(hours2, 2, 10, QChar('0'))
                                      .arg(minutes2, 2, 10, QChar('0'))
                                      .arg(seconds2, 2, 10, QChar('0'));


            ui->timestamp1->setText(timeString1);
            ui->timestamp2->setText(timeString2);
        } else {
            timer->stop();
        }
    });

    timer->start(1000);

    create_and_encrypt();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_3_clicked()
{
    decrypt_file("wannacry.txt");
    QMessageBox::information(this,"Decrypted","Dummy file is decrypted!");
}


void MainWindow::on_pushButton_clicked()
{
    QString text = ui->btcaddr->text();
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(text);
}


void MainWindow::on_pushButton_2_clicked()
{
    QMessageBox::critical(this, "Payment Check", "Payment hasn't been done yet!");
}


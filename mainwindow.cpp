#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <iostream>
#include <locale.h>
#include <string.h>
#include <cstring>
#include <sstream>
#include <ios>
#include <fstream>
#include <vector>
#include <Packet/Packet.h>
#include <Packet/Note.h>
#include <QTextCodec>
#include <QSize>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    char* locale = setlocale(LC_ALL, "Russian");
    // Инициализируем DLL
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Windows-1251"));
    list = new QListWidget(this);
    list->hide();
    list->resize(this->width(), this->height()-100);

    add = new QPushButton(this);
    int w = (this->width()-150)/2;
    add->resize(150, 50);
    add->move(w, 25);
    add->setText("Добавить заметку");

    print = new QPushButton(this);
    print->resize(150, 50);
    print->move(w, 100);
    print->setText("Просмотреть заметки");

    del = new QPushButton(this);
    del->resize(150, 50);
    del->move(w, 175);
    del->setText("Удалить заметку");

    change = new QPushButton(this);
    change->resize(150, 50);
    change->move(w, 250);
    change->setText("Редактировать заметку");

    seeLogs = new QPushButton(this);
    seeLogs->resize(150, 50);
    seeLogs->move(w, 325);
    seeLogs->setText("Просмотреть обращения \n к серверу");

    calendar = new QCalendarWidget(this);
    calendar->resize(this->width(), this->height()-150);
    calendar->hide();

    addButton = new QPushButton(this);
    addButton->resize(150, 50);
    addButton->move(w, this->height()-60);
    addButton->setText("Добавить");
    addButton->hide();

    addField = new QTextEdit(this);
    addField->resize(300, 50);
    addField->move(w-75, this->height()-130);
    addField->hide();

    back = new QPushButton(this);
    back->resize(150, 50);
    back->move(w, this->height()-130);
    back->setText("Назад");
    back->hide();

    deleteButton = new QPushButton(this);
    deleteButton->resize(150, 50);
    deleteButton->move(w+175, this->height()-60);
    deleteButton->setText("Удалить");
    deleteButton->hide();

    editButton = new QPushButton(this);
    editButton->resize(150, 50);
    editButton->move(w+175, this->height()-60);
    editButton->setText("Редактировать");
    editButton->hide();

    editField = new QTextEdit(this);
    editField->resize(200, 50);
    editField->move(w-200, this->height()-60);
    editField->hide();

    message = new QMessageBox(this);

    connect(add, &QPushButton::clicked, this, &MainWindow::addNote);
    connect(print, &QPushButton::clicked, this, &MainWindow::showNotes);
    connect(addButton, &QPushButton::clicked, this, &MainWindow::on_pushButton_clicked);
    connect(back, &QPushButton::clicked, this, &MainWindow::getBack);
    connect(seeLogs, &QPushButton::clicked, this, &MainWindow::showLogs);
    connect(del, &QPushButton::clicked, this, &MainWindow::showDeleteNote);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteNote);
    connect(change, &QPushButton::clicked, this, &MainWindow::showEditNote);
    connect(editButton,&QPushButton::clicked, this, &MainWindow::editNote);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    back->show();
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(&sockAddr, 0, sizeof(sockAddr)); // Каждый байт заполняется 0
    sockAddr.sin_family = PF_INET;
    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sockAddr.sin_port = htons(1234);
    ::connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
    char fc = 65;
    int y = calendar->selectedDate().year();
    int m = calendar->selectedDate().month();
    int d = calendar->selectedDate().day();
    char* msg = addField->toPlainText().toLocal8Bit().data();

    Note note(y,m,d, msg);
    char ymd[252] = { 0 };
    _itoa(note.date.year, ymd, 10);
    strcpy(data, ymd);
    strcat(data, " ");
    _itoa(note.date.month, ymd, 10);
    strcat(data, ymd);
    strcat(data, " ");
    _itoa(note.date.day, ymd, 10);
    strcat(data, ymd);
    strcat(data, " ");
    strcat(data, msg);
    Packet pack(fc, data);
    send(sock, pack.Serialize(), 260, 0);
    recv(sock, szBuffer, 260, 0);
    pack.Deserialize(szBuffer);
    getBack();
    message->setText(QString::fromLocal8Bit(pack.Data));
    message->show();
}



void MainWindow::showButtons(bool b)
{
    if(b)
    {
        add->show();
        print->show();
        del->show();
        change->show();
        seeLogs->show();
    }
    else
    {
        add->hide();
        print->hide();
        del->hide();
        change->hide();
        seeLogs->hide();
    }
}

void MainWindow::showNotes()
{
    back->move((this->width()-150)/2, this->height()-60);
    showButtons(false);
    list->clear();
    list->show();
    back->show();
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(&sockAddr, 0, sizeof(sockAddr)); // Каждый байт заполняется 0
    sockAddr.sin_family = PF_INET;
    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sockAddr.sin_port = htons(1234);
    ::connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
    char fc = 66;
    strcpy(data,"");
    Packet pack(fc, data);
    send(sock, pack.Serialize(), 260, 0);
    pack.Print();
    recv(sock, szBuffer, 260, 0);
    pack.Deserialize(szBuffer);
    int n;
    n = atoi(pack.Data);
    {
        for (int i = 0; i < n; i++)
        {

            recv(sock, szBuffer, 260, 0);
            pack.Deserialize(szBuffer);
            std::stringstream s;
            s << pack.Data;
            Note note(s);
            list->addItem(QString::fromLocal8Bit(note.note));

        }
    }
}

void MainWindow::getBack()
{
    calendar->hide();
    back->hide();
    showButtons(true);
    addButton->hide();
    addField->hide();
    list->hide();
    deleteButton->hide();
    editButton->hide();
    editField->hide();
}

void MainWindow::addNote()
{
    back->move(150, this->height()-60);
    back->show();
    showButtons(false);
    calendar->show();
    addField->show();
    addButton->show();
}

void MainWindow::showLogs()
{
    back->move((this->width()-150)/2, this->height()-60);
    showButtons(false);
    list->clear();
    list->show();
    back->show();
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(&sockAddr, 0, sizeof(sockAddr)); // Каждый байт заполняется 0
    sockAddr.sin_family = PF_INET;
    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sockAddr.sin_port = htons(1234);
    ::connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

    char fc = 69;

    Packet pack(fc, data);
    send(sock, pack.Serialize(), 260, NULL);
    pack.Print();
    recv(sock, szBuffer, 260, NULL);
    pack.Deserialize(szBuffer);
    int n;
    n = atoi(pack.Data);


    for (int i = 0; i < n; i++)
    {

        recv(sock, szBuffer, 260, NULL);
        pack.Deserialize(szBuffer);
        std::stringstream s;
        list->addItem(QString::number(i)+QString(")")+QString::fromLocal8Bit(pack.Data));

    }
}

void MainWindow::showEditNote()
{
    showNotes();
    editButton->show();
    editField->show();
}

void MainWindow::showDeleteNote()
{
    showNotes();
    deleteButton->show();
}

void MainWindow::deleteNote()
{
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(&sockAddr, 0, sizeof(sockAddr)); // Каждый байт заполняется 0
    sockAddr.sin_family = PF_INET;
    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sockAddr.sin_port = htons(1234);
    ::connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
    char fc = 67;
    Packet pack(fc,data);
    int choice = list->selectionModel()->selectedIndexes()[0].row()+1;
    char ch[5];
    _itoa(choice, ch, 10);
    strcpy(pack.Data, ch);
    send(sock, pack.Serialize(), 260, NULL);
    recv(sock, szBuffer, 260, NULL);
    pack.Deserialize(szBuffer);
    getBack();
    message->setText(QString::fromLocal8Bit(pack.Data));
    message->show();
}

void MainWindow::editNote()
{
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(&sockAddr, 0, sizeof(sockAddr)); // Каждый байт заполняется 0
    sockAddr.sin_family = PF_INET;
    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sockAddr.sin_port = htons(1234);
    ::connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
    char fc = 68;

     int choice = list->selectionModel()->selectedIndexes()[0].row()+1;
     Packet pack(fc,data);
     char ch[5];
     _itoa(choice, ch, 10);
     strcpy(pack.Data, ch);
     send(sock, pack.Serialize(), 260, 0);
     recv(sock, szBuffer, 260, 0);
     pack.Deserialize(szBuffer);
     message->setText(QString::fromLocal8Bit(pack.Data));
     message->show();
     char* msg = editField->toPlainText().toLocal8Bit().data();
     std::cout<<msg;
     strcpy(pack.Data, msg);
     send(sock, pack.Serialize(), 260, 0);
\
     getBack();
}

    #ifndef MAINWINDOW_H
    #define MAINWINDOW_H

    #include <QMainWindow>
    #include <stdio.h>
    #include <stdlib.h>
    #include <WinSock2.h>
    #include <string.h>
    #include <iostream>
    #include <locale.h>
    #include <sstream>
    #include <ios>
    #include <cstring>
    #include <QListWidget>
    #include <QPushButton>
    #include <QCalendarWidget>
    #include <QTextEdit>
    #include <QMessageBox>

    QT_BEGIN_NAMESPACE
    namespace Ui { class MainWindow; }
    QT_END_NAMESPACE

    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    private slots:
        void on_pushButton_clicked();


        void showNotes();
        void showButtons(bool b);
        void addNote();
        void getBack();
        void showLogs();
        void deleteNote();
        void showDeleteNote();
        void showEditNote();
        void editNote();
    private:
        Ui::MainWindow *ui;
        SOCKET sock;
        char data[252];
        char szBuffer[260];
        WSADATA wsaData;
        sockaddr_in sockAddr;
        QListWidget* list;
        QPushButton* add;
        QPushButton* print;
        QPushButton* del;
        QPushButton* change;
        QPushButton* seeLogs;
        QCalendarWidget* calendar;
        QPushButton* addButton;
        QTextEdit* addField;
        QPushButton* back;
        QMessageBox* message;
        QPushButton* deleteButton;
        QPushButton* editButton;
        QTextEdit* editField;
    };
    #endif // MAINWINDOW_H

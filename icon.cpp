#include "mainwindow.h"
#include "configwindow.h"
#include "connectdatabase.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    // Создание иконки подключения
    connectionStatusIcon = new QLabel(this);
    QPixmap icon(":/images/connection_icon.png"); // Путь к изображению иконки подключения
    QPixmap resizedIcon = icon.scaled(QSize(20, 20), Qt::KeepAspectRatio); // Изменение размера иконки
    connectionStatusIcon->setPixmap(resizedIcon);
    connectionStatusIcon->setAlignment(Qt::AlignCenter); // Выравнивание иконки по центру

    // Добавление кнопки на главное окно
    QPushButton *configButton = new QPushButton("Настройки", this);

    // Добавление кнопки и иконки на главное окно
    layout->addWidget(configButton);
    layout->addWidget(connectionStatusIcon);

    // Установка межстрочного интервала для макета
    layout->setSpacing(10);

    // Установка выравнивания макета
    layout->setAlignment(Qt::AlignTop);

    // Подключение сигнала нажатия кнопки к слоту
    connect(configButton, &QPushButton::clicked, this, &MainWindow::openConfigWindow);

    // ...
}

MainWindow::~MainWindow()
{
}

void MainWindow::openConfigWindow()
{
    ConfigWindow configWindow;
    configWindow.exec();
}

void MainWindow::checkDatabaseConnection()
{
    // Чтение параметров подключения из файла настроек
    QString configFilePath = "config.conf"; // Путь к файлу настроек
    QSettings settings(configFilePath, QSettings::IniFormat);
    settings.beginGroup("DataBase");

    QString host = settings.value("host").toString();
    int port = settings.value("port").toInt();
    QString dbName = settings.value("name").toString();
    QString user = settings.value("user").toString();
    QString password = settings.value("pass").toString();

    settings.endGroup();

    // Проверка подключения к базе данных
    ConnectDatabase db;
    bool connected = db.connect(host, port, dbName, user, password);

    // Обновление иконки статуса подключения
    updateDatabaseConnectionStatus(connected);

    // Вывод сообщения о результате проверки подключения
    QMessageBox::information(this, tr("Проверка подключения"), connected ? "Подключено" : "Не подключено");
}

void MainWindow::updateDatabaseConnectionStatus(bool connected)
{
    if (connected) {
        QPixmap greenIcon(":/images/green.png"); // Путь к изображению зеленой иконки
        QPixmap resizedIcon = greenIcon.scaled(QSize(20, 20), Qt::KeepAspectRatio); // Изменение размера иконки
        connectionStatusIcon->setPixmap(resizedIcon);
    } else {

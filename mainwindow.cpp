#include<QVBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QDialog>
#include <QFile>
#include <QLabel>
#include <QFormLayout>
#include <QMessageBox>
#include <QPixmap>
#include <QDebug>
#include <QHBoxLayout>
#include <QSplitter>
#include <QtSql/QSqlDatabase>
#include "mainwindow.h"
#include "configwindow.h"
#include "connectdatabase.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setModel( vfvpModel1 );
    setModel2( vfvpModel2 );
    setModel3( vfvpModel3 );
    setModel4( vfvpModel4 );

    vfvpMap.insert(vfvpModel1.charVfvp.vfvp_kod, &vfvpModel1);
    vfvpMap.insert(vfvpModel2.charVfvp.vfvp_kod, &vfvpModel2);
    vfvpMap.insert(vfvpModel3.charVfvp.vfvp_kod, &vfvpModel3);
    vfvpMap.insert(vfvpModel4.charVfvp.vfvp_kod, &vfvpModel4);

    //vfvpMap.insert(vfvpmodel.charVfvp.vfvp_kod, &vfvpmodel);
    createTreeView();
    createTabWidget();
    createCustomMenu();

    // добавляю горизонтальный разделитель
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(treeView);
    splitter->addWidget(tabWidget);
    splitter->setStretchFactor(1, 1);
    splitter->setCollapsible(0, false);
    splitter->setCollapsible(1, false);

//    QWidget *centralWidget = new QWidget(this);
//    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
//    layout->addWidget(splitter);
   // setCentralWidget(centralWidget);

    // подключаем слот к сигналу clicked дерева
//   connect(treeView, &QTreeView::clicked, this, &MainWindow::onTreeViewItemClicked);
    connect( treeView, SIGNAL( clicked(QModelIndex)),
            this, SLOT( onTreeViewItemClicked(QModelIndex))
             );

    this->setWindowTitle("Модель данных ТВФ в части ВФВП");

    /// кнопка нового окна
    button = new QPushButton(this);
    button->setToolTip("Настройка сети");
    button->setIcon(QIcon(":/images/images/blue.png")); // Устанавливаем изображение иконки
    button->setIconSize(QSize(20, 20));
    int buttonSize = 20;  // Размер кнопки
    button->setFixedSize(buttonSize, buttonSize);
    int borderRadius = buttonSize / 2;  // Радиус скругления
    button->setStyleSheet(QString("QPushButton { border-radius: %1px; }").arg(borderRadius));

/// кликнули сдесь - запустили новое окно и сигнал -settingsSaved() но при условии
///  когда вызывается saveToFile() ну по сути только когда мы нажимаем на кнопку сохранить
    connect(button, &QPushButton::clicked, this, &MainWindow::openNewWindow);

//    QWidget *centralWidget = new QWidget(this);
//    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
//    layout->addWidget(splitter);
//    QVBoxLayout *layoutV = new QVBoxLayout(centralWidget);
   connectionStatusIcon = new QLabel(this);
    QPixmap   grayIcon(":/images/images/stat.png"); // Путь к изображению серой иконки
    connectionStatusIcon->setPixmap(grayIcon);
//    // Создание горизонтального контейнера для размещения кнопки и иконки
//    QHBoxLayout* layoutH = new QHBoxLayout;
//    layoutH->addStretch();
//    layoutH->addWidget( connectionStatusIcon, 0, Qt::AlignRight );
//    layoutH->addWidget( button, 0, Qt::AlignRight);
//    layoutV->addLayout( layoutH );
////    layoutV->addWidget(tabWidget);

//    centralWidget->setLayout(layoutV);

//    setCentralWidget(centralWidget);
    // Создаем вертикальный контейнер для размещения кнопки и иконки
    QHBoxLayout *buttonLayout = new QHBoxLayout;
   buttonLayout->addStretch(1);
    buttonLayout->addWidget(connectionStatusIcon, 0, Qt::AlignRight);
    buttonLayout->addWidget(button, 0, Qt::AlignRight);
        // Создаем главный вертикальный контейнер
        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->addLayout(buttonLayout);
        mainLayout->addWidget(splitter);

        QWidget *centralWidget = new QWidget(this);
        centralWidget->setLayout(mainLayout);
        setCentralWidget(centralWidget);


checkConfigFile();
closeWindowCloseDbConnection();
}
MainWindow::~MainWindow()
{
}
void MainWindow::createCustomMenu()
{
    // Create File menu and actions
    QMenu *fileMenu = menuBar()->addMenu("&Файл");
/// пока оставил, может будет нужно
//    QAction *newAction = new QAction(tr("&Создать"), this);
//    fileMenu->addAction(newAction);

//    QAction *openAction = new QAction(tr("&Открыть"), this);
//    fileMenu->addAction(openAction);

//    QAction *saveAction = new QAction(tr("&Сохранить"), this);
//    fileMenu->addAction(saveAction);
    QAction *settingAction = new QAction(tr("&Настройки"), this);
    fileMenu->addAction(settingAction);

    QAction *exitAction = new QAction(("&Выход"), this);
    fileMenu->addAction(exitAction);

    // Подключаю сигнал слот
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    connect(settingAction, &QAction::triggered, this, &MainWindow::openNewWindow);
}

/// отображение нового окна, ныряем в ConfigWindow (обновляем данные если данные обновились запускаем updateConnection
void MainWindow::openNewWindow()
{
    configWindow = new ConfigWindow(this);
    connect(configWindow, &ConfigWindow::settingsSaved, this, &MainWindow::updateConnection, Qt::QueuedConnection);
    configWindow->exec();
}

/// Обновление подключения к базе данных (запускаем базу данных через проверку checkDatabaseConnection
/// затем checkDatabaseConnection через connected запускаем проверку updateDatabaseConnectionStatus(bool connected) и меняем цвет иконки
///  затем мы у финиша-  проверка завершена, статус установлен, приходим в конец конструктора )
void MainWindow::updateConnection()
{
    connectDatabase->checkDatabaseConnection(); // Обновление подключения к базе данных
}
// смена статусов подключения
void MainWindow::updateDatabaseConnectionStatus(bool connected)
{
    if (connected) {
            QPixmap greenIcon(":/images/images/connect16.png"); // Путь к изображению зеленой иконки
            connectionStatusIcon->setPixmap(greenIcon);
        } else {
            QPixmap redIcon(":/images/images/no-connection16.png"); // Путь к изображению красной иконки
            connectionStatusIcon->setPixmap(redIcon);
        }

}
// Закрываем базу данных при закрытии главного окна
void MainWindow::closeWindowCloseDbConnection()
{
    // Закрытие соединения с базой данных
    QSqlDatabase::database().close();
    QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
}

// Проверяем существует ли файл настроек
void MainWindow::checkConfigFile()
{
    QString configFilePath = "config.conf";

    // Проверяем наличие файла
    QFile file(configFilePath);
    if (!file.exists()) {
        configWindow->createDefaultSettings(configFilePath);
        QMessageBox::warning(nullptr, "Внимание!", "Файл настроек сети не найден. Будет создан файл настроек по умолчанию! "
                                                       "Если соединение не установлено, то требуется внести изменения в настройки конфигурации сети "
                                                       " в меню настроек!");

    }
    connectDatabase = new ConnectDatabase(this);
    connect(connectDatabase, &ConnectDatabase::databaseConnected, this, &MainWindow::updateDatabaseConnectionStatus);
    connectDatabase->checkDatabaseConnection();

}
// закрытие соединения с базой
void MainWindow::closeEvent(QCloseEvent *event)
{
  closeWindowCloseDbConnection();
  QMainWindow::closeEvent(event);
}

/// РАБОТАЕТ!!!
void MainWindow::createTreeView()
{
    model = new QStandardItemModel(this);
    QStandardItem* rootItem = model->invisibleRootItem();

    // Создал контейнер для помещения туда элементов верхнего уровня
    QList<QStandardItem*> topLevelItems;

    foreach (VFVPModel* vfvpModel, vfvpMap)
    {
        ///проверяю в буле ниже  есть ли в мапе элемент у которого  код charVfvp.vfvp_kod
        ///совпадает с кодом  элемента charVfvp.vfvp_org
        /// если есть значит текущий элемент имеет родителя
        /// если условие выше соблюдается то добавляем элемент parentItem в лист парентов
        if (!hasParent(vfvpModel))
        {
            QStandardItem* parentItem = new QStandardItem(vfvpModel->charVfvp.vfvp_name);
            parentItem->setData(vfvpModel->charVfvp.vfvp_kod, Qt::UserRole);
            topLevelItems.append(parentItem);
        }
    }

   /// Добавляем каждый элемент верхнего уровня в модель и делаем их рутами
    foreach (QStandardItem* item, topLevelItems)
    {
        rootItem->appendRow(item);
    }

    ///   если лист не пустой то создаем дочерние элементы
    while (!topLevelItems.isEmpty())
    {
        /// извлекаем первый элемент из листа удаляем его от туда и  присваеваем его parentItem
        QStandardItem* parentItem = topLevelItems.takeFirst();
        createChildItems(parentItem, topLevelItems);
    }

    treeView = new QTreeView(this);
    treeView->setModel(model);
    treeView->setHeaderHidden(true);
    treeView->setSelectionMode(QAbstractItemView::SingleSelection);
}

void MainWindow::createChildItems(QStandardItem* parentItem, QList<QStandardItem*>& topLevelItems)
{
    //
    QString parentCode = parentItem->data(Qt::UserRole).toString();

    foreach (VFVPModel* childModel, vfvpMap)
    {
        if (childModel->charVfvp.vfvp_org == parentCode)
        {
            QStandardItem* childItem = new QStandardItem(childModel->charVfvp.vfvp_name);
            childItem->setData(childModel->charVfvp.vfvp_kod, Qt::UserRole);
            parentItem->appendRow(childItem);
            topLevelItems.append(childItem);
        }
    }
}

bool MainWindow::hasParent(VFVPModel* vfvpModel)
{
    QString parentCode = vfvpModel->charVfvp.vfvp_org;

    foreach (VFVPModel* otherModel, vfvpMap)
    {
        if (otherModel->charVfvp.vfvp_kod == parentCode)
            return true;
    }

    return false;
}

void MainWindow::createTabWidget()
{
    tabWidget = new QTabWidget(this);

    QStringList tabNames = {
        "Характеристики ВФВП [ vfvp ]",
        "Положение воинского формирования [ pvfvp ]",
        "Направление действий воинского формирования [ ndvfvp ]",
        "Личный состав [ lsvfvp ]",
        "ВВТ [ vvtvfvp ]",
        "Картографические данные [ kdou ]"
    };

    QStringList labelNames = {
        "Учетный номер ВФВП",
        "Источник информации",
        "Дата и время состояния",
        "Номер ВФВП",
        "Наименование типового ВФВП",
        "Вид ВС",
        "Род войск",
        "Организационно-штатная принадлежность",
        "Государственная принадлежность (по ОКСМ)",
        "Государственная принадлежность (по КСМТ)",
        "Уровень ВФСВ",
        "Тип объекта разведки",
        "Наименование воинского формирования",
        "Сокращенное наименование",
        "Код в ЕСУ ТЗ",
        "Время обнаружения",
        "Номер цели",
        "Новый номер цели",
        "Степень укрытости",
        "Прогноз",
        "Истинность",
        "Достоверность",
        "Источник разведки",
        "Признак нахождения в боевом порядке",
        "Признак важности объекта",
        "Примечание",
        "Характер действий"
        "Боеспособность",
        "Высота"

    };
//    QWidget* MainWindow::createScrollableTab(const QString& tabName, const QStringList& labelNames, const QStringList& fieldNames, bool readOnly)
//    {
//        QWidget* tabWidget = new QWidget(this);
//        QScrollArea* scrollArea = new QScrollArea(tabWidget);
//        QWidget* scrollWidget = new QWidget(scrollArea);
//        QVBoxLayout* scrollLayout = new QVBoxLayout(scrollWidget);

//        scrollArea->setWidgetResizable(true);
//        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
//        tabWidget->setLayout(new QVBoxLayout(tabWidget));
//        tabWidget->layout()->addWidget(scrollArea);

//        scrollArea->setWidget(scrollWidget);

//        for (int i = 0; i < labelNames.size(); i++) {
//            QLabel* label = new QLabel(labelNames[i], scrollWidget);
//            QTextEdit* textEdit = new QTextEdit(scrollWidget);

//            textEdit->setObjectName(QString("TextEdit%1%2").arg(fieldNames[i]).arg(tabName));
//            textEdit->setReadOnly(readOnly);

//            scrollLayout->addWidget(label);
//            scrollLayout->addWidget(textEdit);
//        }

//        scrollLayout->addStretch(1);

//        return tabWidget;
//    }
//    void MainWindow::createTabWidget()
//    {
//        tabWidget = new QTabWidget(this);

//        QStringList tabNames = {
//            "Характеристики",
//            "Положение",
//            "Направление",
//            "Ли",
//            "В",
//            "Ка данные"
//        };

//        QStringList labelNamesTab1 = {
//            "Учетный номер",
//            "Источник информации",
//            "Дата и время состояния",
//            // ... добавьте остальные названия меток здесь ...
//        };

//        QStringList fieldNamesTab1 = {
//            "Field1",
//            "Field2",
//            "Field3",
//            // ... добавьте остальные названия полей здесь ...
//        };

//        QStringList labelNamesTab2 = {
//            "Наименование блока данных координат",
//            "Геодезическая широта",
//            "Геодезическая долгота",
//            "Высота",
//            "Пояснение:",
//            // ... добавьте остальные названия меток здесь ...
//        };

//        QStringList fieldNamesTab2 = {
//            "Field1",
//            "Field2",
//            "Field3",
//            // ... добавьте остальные названия полей здесь ...
//        };

//        // Создание вкладок
//        tab1 = createScrollableTab(tabNames[0], labelNamesTab1, fieldNamesTab1, true);
//        tab2 = createScrollableTab(tabNames[1], labelNamesTab2, fieldNamesTab2, false);
//        // ... создание остальных вкладок ...

//        // Добавление вкладок в QTabWidget
//        tabWidget->addTab(tab1, tabNames[0]);
//        tabWidget->addTab(tab2, tabNames[1]);
//        // ... добавление остальных вкладок ...

//        //



    tab1 = createScrollableTab(tabNames[0], labelNames, true);
    tab2 = createScrollableTab(tabNames[1], labelNames, false);
    tab3 = createScrollableTab(tabNames[2], labelNames, false);
    tab4 = createScrollableTab(tabNames[3], labelNames, false);
    tab5 = createScrollableTab(tabNames[4], labelNames, false);
    tab6 = createScrollableTab(tabNames[5], labelNames, false);

    tabWidget->addTab(tab1, tabNames[0]);
    tabWidget->addTab(tab2, tabNames[1]);
    tabWidget->addTab(tab3, tabNames[2]);
    tabWidget->addTab(tab4, tabNames[3]);
    tabWidget->addTab(tab5, tabNames[4]);
    tabWidget->addTab(tab6, tabNames[5]);

    // Дополнительная логика и соединения сигналов, если необходимо

}
QWidget* MainWindow::createScrollableTab(const QString& tabName, const QStringList& labelNames, bool readOnly)
{
    QWidget* tabWidget = new QWidget(this);
    QScrollArea* scrollArea = new QScrollArea(tabWidget);
    QWidget* scrollWidget = new QWidget(scrollArea);
    QVBoxLayout* scrollLayout = new QVBoxLayout(scrollWidget);

    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    tabWidget->setLayout(new QVBoxLayout(tabWidget));
    tabWidget->layout()->addWidget(scrollArea);

    scrollArea->setWidget(scrollWidget);

    for (const QString& labelName : labelNames) {
        QLabel* label = new QLabel(labelName, scrollWidget);
        QTextEdit* textEdit = new QTextEdit(scrollWidget);

        textEdit->setObjectName(QString("TextEdit%1Tab1").arg(labelName));
        textEdit->setReadOnly(readOnly);

        scrollLayout->addWidget(label);
        scrollLayout->addWidget(textEdit);
    }

    scrollLayout->addStretch(1);

    return tabWidget;
}


//void MainWindow::createTabWidget()
//{
//    // добавляю вкладки
//    tabWidget = new QTabWidget(this);
//    tab1 = new QWidget(tabWidget);
//    tab2 = new QWidget(tabWidget);
//    tab3 = new QWidget(tabWidget);
//    tab4 = new QWidget(tabWidget);
//    tab5 = new QWidget(tabWidget);
//    tab6 = new QWidget(tabWidget);

//    /// характеристики ВФВП [ vfvp ]
//    /// положение воинского формирования [ pvfvp ]
//    /// направление действий воинского формирования [ ndvfvp ]
//    /// картографические данные [ kdou ]
//    /// личный состав [ lsvfvp ]
//    /// ВВТ [ vvtvfvp ]

//    tabWidget->addTab(tab1, ("Характеристики ВФВП [ vfvp ]"));
//    tabWidget->addTab(tab2, ("Положение воинского формирования [ pvfvp ]"));
//    tabWidget->addTab(tab3, ("Направление действий воинского формирования [ ndvfvp ]"));
//    tabWidget->addTab(tab4, ("Личный состав [ lsvfvp ]"));
//    tabWidget->addTab(tab5, ("ВВТ [ vvtvfvp ]"));
//    tabWidget->addTab(tab6, ("Картографические данные [ kdou ]"));
//    //tabWidget->addTab(tab6, ("Картографические данные [ kdou ] ( txf )"));

//    // добавляю QScrollArea для вертикального скроллинга
//        QScrollArea* scrollArea = new QScrollArea(tab1);
//        scrollArea->setWidgetResizable(true);
//        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
//        tab1->setLayout(new QVBoxLayout(tab1));
//        tab1->layout()->addWidget(scrollArea);


//            QWidget *scrollWidget = new QWidget(scrollArea);
//            scrollArea->setWidget(scrollWidget);

//            QVBoxLayout *scrollLayout = new QVBoxLayout(scrollWidget);
//            scrollWidget->setLayout(scrollLayout);

//            leTab1Edit1 = new QLineEdit("lineEdit",scrollWidget);
//            scrollWidget->layout()->addWidget(leTab1Edit1);


//            textTab1Edit1 = new QTextEdit(scrollWidget);
//            textTab1Edit1->setObjectName("TextEdit1Tab1");
//            scrollWidget->layout()->addWidget(textTab1Edit1);
//            textTab1Edit2 = new QTextEdit(scrollWidget);
//            textTab1Edit2->setObjectName("TextEdit2Tab1");
//            scrollWidget->layout()->addWidget(textTab1Edit2);
//            textTab1Edit3 = new QTextEdit(scrollWidget);
//            textTab1Edit3->setObjectName("TextEdit3Tab1");
//            scrollWidget->layout()->addWidget(textTab1Edit3);
//            textTab1Edit4 = new QTextEdit(scrollWidget);
//            textTab1Edit4->setObjectName("TextEdit4Tab1");
//            scrollWidget->layout()->addWidget(textTab1Edit4);
//            textTab1Edit5 = new QTextEdit(scrollWidget);
//            textTab1Edit5->setObjectName("TextEdit5Tab1");
//            scrollWidget->layout()->addWidget(textTab1Edit5);
//            textTab1Edit6 = new QTextEdit(scrollWidget);
//            textTab1Edit6->setObjectName("TextEdit6Tab1");
//            scrollWidget->layout()->addWidget(textTab1Edit6);
//            textTab1Edit7 = new QTextEdit(scrollWidget);
//            textTab1Edit7->setObjectName("TextEdit7Tab1");
//            scrollWidget->layout()->addWidget(textTab1Edit7);
//            textTab1Edit8 = new QTextEdit(scrollWidget);
//            textTab1Edit8->setObjectName("TextEdit8Tab1");
//            scrollWidget->layout()->addWidget(textTab1Edit8);
//            textTab1Edit9 = new QTextEdit(scrollWidget);
//            textTab1Edit9->setObjectName("TextEdit9Tab1");
//            scrollWidget->layout()->addWidget(textTab1Edit9);
//            textTab1Edit10 = new QTextEdit(scrollWidget);
//            textTab1Edit10->setObjectName("TextEdit10Tab1");
//            scrollWidget->layout()->addWidget(textTab1Edit10);
//            textTab1Edit11 = new QTextEdit(scrollWidget);
//            textTab1Edit11->setObjectName("TextEdit11Tab1");
//            scrollWidget->layout()->addWidget(textTab1Edit11);
//            textTab1Edit12 = new QTextEdit(scrollWidget);
//            textTab1Edit12->setObjectName("TextEdit12Tab1");
//            scrollWidget->layout()->addWidget(textTab1Edit12);
//            textTab1Edit13 = new QTextEdit(scrollWidget);
//            textTab1Edit13->setObjectName("TextEdit13Tab1");
//            scrollWidget->layout()->addWidget(textTab1Edit13);
//            textTab1Edit14 = new QTextEdit(scrollWidget);
//            textTab1Edit14->setObjectName("TextEdit14Tab1");
//              scrollWidget->layout()->addWidget(textTab1Edit14);
//            textTab1Edit15 = new QTextEdit(scrollWidget);
//            textTab1Edit15->setObjectName("TextEdit15Tab1");
//              scrollWidget->layout()->addWidget(textTab1Edit15);
//            textTab1Edit16 = new QTextEdit(scrollWidget);
//            textTab1Edit16->setObjectName("TextEdit16Tab1");
//              scrollWidget->layout()->addWidget(textTab1Edit16);
//            textTab1Edit17 = new QTextEdit(scrollWidget);
//            textTab1Edit17->setObjectName("TextEdit17Tab1");
//              scrollWidget->layout()->addWidget(textTab1Edit17);
//            textTab1Edit18 = new QTextEdit(scrollWidget);
//            textTab1Edit18->setObjectName("TextEdit18Tab1");
//              scrollWidget->layout()->addWidget(textTab1Edit18);
//            textTab1Edit19 = new QTextEdit(scrollWidget);
//            textTab1Edit19->setObjectName("TextEdit19Tab1");
//              scrollWidget->layout()->addWidget(textTab1Edit19);
//            textTab1Edit20 = new QTextEdit(scrollWidget);
//            textTab1Edit20->setObjectName("TextEdit20Tab1");
//              scrollWidget->layout()->addWidget(textTab1Edit20);
//            textTab1Edit21 = new QTextEdit(scrollWidget);
//            textTab1Edit21->setObjectName("TextEdit21Tab1");
//              scrollWidget->layout()->addWidget(textTab1Edit21);
//            textTab1Edit22 = new QTextEdit(scrollWidget);
//            textTab1Edit22->setObjectName("TextEdit22Tab1");
//              scrollWidget->layout()->addWidget(textTab1Edit22);
//            textTab1Edit23 = new QTextEdit(scrollWidget);
//            textTab1Edit23->setObjectName("TextEdit23Tab1");
//              scrollWidget->layout()->addWidget(textTab1Edit23);
//            textTab1Edit24 = new QTextEdit(scrollWidget);
//            textTab1Edit24->setObjectName("TextEdit24Tab1");
//              scrollWidget->layout()->addWidget(textTab1Edit24);
//            textTab1Edit25 = new QTextEdit(scrollWidget);
//            textTab1Edit25->setObjectName("TextEdit25Tab1");
//              scrollWidget->layout()->addWidget(textTab1Edit25);
//            textTab1Edit26 = new QTextEdit(scrollWidget);
//            textTab1Edit26->setObjectName("TextEdit26Tab1");
//              scrollWidget->layout()->addWidget(textTab1Edit26);
//            textTab1Edit27 = new QTextEdit(scrollWidget);
//            textTab1Edit27->setObjectName("TextEdit27Tab1");
//              scrollWidget->layout()->addWidget(textTab1Edit27);
//            textTab1Edit28 = new QTextEdit(scrollWidget);
//            textTab1Edit28->setObjectName("TextEdit28Tab1");
//              scrollWidget->layout()->addWidget(textTab1Edit28);
//            textTab1Edit29 = new QTextEdit(scrollWidget);
//            textTab1Edit29->setObjectName("TextEdit29Tab1");
//              scrollWidget->layout()->addWidget(textTab1Edit29);


////    // добавляю подписи лейблы
////                QStringList() << "vfvp_kod"        /// Учетный номер ВФВП
////                              << "vfvp_dlkod"      /// Источник информации
////                              << "vfvp_dt"         /// Дата и время состояния
////                              << "vfvp_num"        /// Номер ВФВП
////                              << "ktvfig_kod"      /// Наименование типового ВФВП
////                              << "kvvsig_kod"      /// Вид ВС
////                              << "krvig_kod"       /// Род войск
////                              << "vfvp_org"        /// Организационно-штатная принадлежность
////                              << "oksm_kod"        /// Государственная принадлежность (по ОКСМ)
////                              << "ksmt_kod"        /// Государственная принадлежность (по КСМТ)
////                              << "kuvf_kod"        /// Уровень ВФСВ
////                              << "ktor_kod"        /// Тип объекта разведки
////                              << "vfvp_name"       /// Наименование
////                              << "vfvp_sname"      /// Сокращенное наименование
////                              << "vfvp_kod_esutz"  /// Код в ЕСУ ТЗ
////                              << "vfvp_vro"        /// Время обнаружения
////                              << "vfvp_numc"       /// Номер цели
////                              << "vfvp_numcnew"    /// Новый номер цели
////                              << "ksuo_kod"        /// Степень укрытости
////                              << "kpo_kod"         /// Прогноз
////                              << "kio_kod"         /// Истинность
////                              << "kdo_kod"         /// Достоверность
////                              << "kir_kod"         /// Источник разведки
////                              << "vfvp_bp"         /// Признак нахождения в боевом порядке
////                              << "vfvp_importance" /// Признак важности объекта
////                              << "vfvp_info"       /// Примечание
////                              ;

//              labela1Tab1 = new QLabel(("Учетный номер ВФВП"), scrollWidget);
//              scrollLayout->addWidget(labela1Tab1);
//              scrollLayout->addWidget(textTab1Edit1);


//              labela2Tab1 = new QLabel(("Источник информации"), scrollWidget);
//              scrollLayout->addWidget(labela2Tab1);
//              scrollLayout->addWidget(textTab1Edit2);

//              labela3Tab1 = new QLabel(("Дата и время состояния"), scrollWidget);
//              scrollLayout->addWidget(labela3Tab1);
//              scrollLayout->addWidget(textTab1Edit3);

//              labela4Tab1 = new QLabel(("Номер ВФВП"), scrollWidget);
//              scrollLayout->addWidget(labela4Tab1);
//              scrollLayout->addWidget(textTab1Edit4);

//              labela5Tab1 = new QLabel(("Наименование типового ВФВП"), scrollWidget);
//              scrollLayout->addWidget(labela5Tab1);
//              scrollLayout->addWidget(textTab1Edit5);

//              labela6Tab1 = new QLabel(("Вид ВС"), scrollWidget);
//              scrollLayout->addWidget(labela6Tab1);
//              scrollLayout->addWidget(textTab1Edit6);

//              labela7Tab1 = new QLabel(("Род войск"), scrollWidget);
//              scrollLayout->addWidget(labela7Tab1);
//              scrollLayout->addWidget(textTab1Edit7);

//              labela8Tab1 = new QLabel(("Организационно-штатная принадлежность"), scrollWidget);
//              scrollLayout->addWidget(labela8Tab1);
//              scrollLayout->addWidget(textTab1Edit8);

//              labela9Tab1 = new QLabel(("Государственная принадлежность (по ОКСМ)"), scrollWidget);
//              scrollLayout->addWidget(labela9Tab1);
//              scrollLayout->addWidget(textTab1Edit9);

//              labela10Tab1 = new QLabel(("Государственная принадлежность (по КСМТ)"), scrollWidget);
//              scrollLayout->addWidget(labela10Tab1);
//              scrollLayout->addWidget(textTab1Edit10);

//              labela11Tab1 = new QLabel(("Уровень ВФСВ"), scrollWidget);
//              scrollLayout->addWidget(labela11Tab1);
//              scrollLayout->addWidget(textTab1Edit11);

//              labela12Tab1 = new QLabel(("Тип объекта разведки"), scrollWidget);
//              scrollLayout->addWidget(labela12Tab1);
//              scrollLayout->addWidget(textTab1Edit12);

//              labela13Tab1 = new QLabel(("Наименование воинского формирования"), scrollWidget);
//              scrollLayout->addWidget(labela13Tab1);
//              scrollLayout->addWidget(textTab1Edit13);

//              labela14Tab1 = new QLabel(("Сокращенное наименование"), scrollWidget);
//              scrollLayout->addWidget(labela14Tab1);
//              scrollLayout->addWidget(textTab1Edit14);

//              labela15Tab1 = new QLabel(("Код в ЕСУ ТЗ"), scrollWidget);
//              scrollLayout->addWidget(labela15Tab1);
//              scrollLayout->addWidget(textTab1Edit15);

//              labela16Tab1 = new QLabel(("Время обнаружения"), scrollWidget);
//              scrollLayout->addWidget(labela16Tab1);
//              scrollLayout->addWidget(textTab1Edit16);

//              labela17Tab1 = new QLabel(("Номер цели"), scrollWidget);
//              scrollLayout->addWidget(labela17Tab1);
//              scrollLayout->addWidget(textTab1Edit17);

//              labela18Tab1 = new QLabel(("Новый номер цели"), scrollWidget);
//              scrollLayout->addWidget(labela18Tab1);
//              scrollLayout->addWidget(textTab1Edit18);

//              labela19Tab1 = new QLabel(("Степень укрытости"), scrollWidget);
//              scrollLayout->addWidget(labela19Tab1);
//              scrollLayout->addWidget(textTab1Edit19);

//              labela20Tab1 = new QLabel(("Прогноз"), scrollWidget);
//              scrollLayout->addWidget(labela20Tab1);
//              scrollLayout->addWidget(textTab1Edit20);

//              labela21Tab1 = new QLabel(("Истинность"), scrollWidget);
//              scrollLayout->addWidget(labela21Tab1);
//              scrollLayout->addWidget(textTab1Edit21);

//              labela22Tab1 = new QLabel(("Достоверность"), scrollWidget);
//              scrollLayout->addWidget(labela22Tab1);
//              scrollLayout->addWidget(textTab1Edit22);

//              labela23Tab1 = new QLabel(("Источник разведки"), scrollWidget);
//              scrollLayout->addWidget(labela23Tab1);
//              scrollLayout->addWidget(textTab1Edit23);

//              labela24Tab1 = new QLabel(("Признак нахождения в боевом порядке"), scrollWidget);
//              scrollLayout->addWidget(labela24Tab1);
//              scrollLayout->addWidget(textTab1Edit24);

//              labela25Tab1 = new QLabel(("Признак важности объекта"), scrollWidget);
//              scrollLayout->addWidget(labela25Tab1);
//              scrollLayout->addWidget(textTab1Edit25);

//              labela26Tab1 = new QLabel(("Примечание"), scrollWidget);
//              scrollLayout->addWidget(labela26Tab1);
//              scrollLayout->addWidget(textTab1Edit26);

//              labela27Tab1 = new QLabel(("Характер действий"), scrollWidget);
//              scrollLayout->addWidget(labela27Tab1);
//              scrollLayout->addWidget(textTab1Edit27);

//              labela28Tab1 = new QLabel(("Боеспособность"), scrollWidget);
//              scrollLayout->addWidget(labela28Tab1);
//              scrollLayout->addWidget(textTab1Edit28);

//              labela29Tab1 = new QLabel(("Высота"), scrollWidget);
//              scrollLayout->addWidget(labela29Tab1);
//              scrollLayout->addWidget(textTab1Edit29);

////    // делаю поля только для чтения
//    textTab1Edit1->setReadOnly(true);
//    textTab1Edit2->setReadOnly(true);
//    textTab1Edit3->setReadOnly(true);
//    textTab1Edit4->setReadOnly(true);
//    textTab1Edit5->setReadOnly(true);
//    textTab1Edit6->setReadOnly(true);
//    textTab1Edit7->setReadOnly(true);
//    textTab1Edit8->setReadOnly(true);
//    textTab1Edit9->setReadOnly(true);
//    textTab1Edit10->setReadOnly(true);
//    textTab1Edit11->setReadOnly(true);
//    textTab1Edit12->setReadOnly(true);
//    textTab1Edit13->setReadOnly(true);
//    textTab1Edit14->setReadOnly(true);
//    textTab1Edit15->setReadOnly(true);
//    textTab1Edit16->setReadOnly(true);
//    textTab1Edit17->setReadOnly(true);
//    textTab1Edit18->setReadOnly(true);
//    textTab1Edit19->setReadOnly(true);
//    textTab1Edit20->setReadOnly(true);
//    textTab1Edit21->setReadOnly(true);
//    textTab1Edit22->setReadOnly(true);
//    textTab1Edit23->setReadOnly(true);
//    textTab1Edit24->setReadOnly(true);
//    textTab1Edit25->setReadOnly(true);
//    textTab1Edit26->setReadOnly(true);
//    textTab1Edit27->setReadOnly(true);
//    textTab1Edit28->setReadOnly(true);
//    textTab1Edit29->setReadOnly(true);


//    QCheckBox *hideEmptyTextEdit = new QCheckBox("Скрыть пустые поля", scrollWidget);
//   // tabWidget->setCornerWidget(hideEmptyTextEdit, Qt::BottomRightCorner);
//   // tabWidget->setCornerWidget(hideEmptyTextEdit, Qt::AlignTop);
//    scrollLayout->addWidget(hideEmptyTextEdit, 2, Qt::AlignRight);
//scrollLayout->addStretch(1);

//connect(hideEmptyTextEdit, &QCheckBox::clicked, this, &MainWindow::onHideEmptyClicked);

//    QVBoxLayout *tab2Layout = new QVBoxLayout(tab2);

//    textTab2Edit1 = new QTextEdit(tab2);
//    textTab2Edit1->setObjectName("TextEdit1Tab2");
//    textTab2Edit2 = new QTextEdit(tab2);
//    textTab2Edit2->setObjectName("TextEdit2Tab2");
//    textTab2Edit3 = new QTextEdit(tab2);
//    textTab2Edit3->setObjectName("TextEdit3Tab2");
//    textTab2Edit4 = new QTextEdit(tab2);
//    textTab2Edit4->setObjectName("TextEdit4Tab2");
//    textTab2Edit5 = new QTextEdit(tab2);
//    textTab2Edit5->setObjectName("TextEdit5Tab2");

//tab2->setLayout( tab2Layout );

//    QLabel *labelb1Tab2 = new QLabel(("Наименование блока данных координат"), tab2);
//    QLabel *labelb2Tab2 = new QLabel(("Геодезическая широта"), tab2);
//    QLabel *labelb3Tab2 = new QLabel(("Геодезическая долгота"), tab2);
//    QLabel *labelb4Tab2 = new QLabel(("Высота"), tab2);
//    QLabel *labelb5Tab2 = new QLabel(("Пояснение:"), tab2);

//    textTab2Edit1->setReadOnly(true);
//    textTab2Edit2->setReadOnly(true);
//    textTab2Edit3->setReadOnly(true);
//    textTab2Edit4->setReadOnly(true);
//    textTab2Edit5->setReadOnly(true);

//    tab2Layout->addWidget(labelb1Tab2);
//    tab2Layout->addWidget(textTab2Edit1);
//    tab2Layout->addWidget(labelb2Tab2);
//    tab2Layout->addWidget(textTab2Edit2);
//    tab2Layout->addWidget(labelb3Tab2);
//    tab2Layout->addWidget(textTab2Edit3);
//    tab2Layout->addWidget(labelb4Tab2);
//    tab2Layout->addWidget(textTab2Edit4);
//    tab2Layout->addWidget(labelb5Tab2);
//    tab2Layout->addWidget(textTab2Edit5);

//    tab2Layout->addStretch(1);

//    QVBoxLayout *tab4Layout = new QVBoxLayout(tab4);
//    textTab4Edit1 = new QTextEdit(tab4);
//    textTab4Edit1->setObjectName("TextEdit1Tab4");
//    textTab4Edit2 = new QTextEdit(tab4);
//    textTab4Edit2->setObjectName("TextEdit2Tab4");
//    textTab4Edit3 = new QTextEdit(tab4);
//    textTab4Edit3->setObjectName("TextEdit3Tab4");
//    textTab4Edit4 = new QTextEdit(tab4);
//    textTab4Edit4->setObjectName("TextEdit4Tab4");


//tab4->setLayout( tab4Layout );

//    QLabel *labelc1Tab4 = new QLabel(("Наименование блока личного состава"), tab4);
//    QLabel *labelc2Tab4 = new QLabel(("Колличество по штату личного состава"), tab4);
//    QLabel *labelc3Tab4 = new QLabel(("Колличество в наличии личного состава"), tab4);
//    QLabel *labelc4Tab4 = new QLabel(("Укомплектованность личного состава"), tab4);

//    textTab4Edit1->setReadOnly(true);
//    textTab4Edit2->setReadOnly(true);
//    textTab4Edit3->setReadOnly(true);
//    textTab4Edit4->setReadOnly(true);

//    // устанавливаю layouts
//    tab4Layout->addWidget(labelc1Tab4);
//    tab4Layout->addWidget(textTab4Edit1);
//    tab4Layout->addWidget(labelc2Tab4);
//    tab4Layout->addWidget(textTab4Edit2);
//    tab4Layout->addWidget(labelc3Tab4);
//    tab4Layout->addWidget(textTab4Edit3);
//    tab4Layout->addWidget(labelc4Tab4);
//    tab4Layout->addWidget(textTab4Edit4);

//    tab4Layout->addStretch(1);

//    QVBoxLayout *tab5Layout = new QVBoxLayout(tab5);
//    textTab5Edit1 = new QTextEdit(tab5);
//    textTab5Edit1->setObjectName("TextEdit1Tab5");
//    textTab5Edit2 = new QTextEdit(tab5);
//    textTab5Edit2->setObjectName("TextEdit2Tab5");
//    textTab5Edit3 = new QTextEdit(tab5);
//    textTab5Edit3->setObjectName("TextEdit3Tab5");
//    textTab5Edit4 = new QTextEdit(tab5);
//    textTab5Edit4->setObjectName("TextEdit4Tab5");
//    textTab5Edit5 = new QTextEdit(tab5);
//    textTab5Edit5->setObjectName("TextEdit5Tab5");

//tab5->setLayout( tab5Layout );


//    QLabel *labelc1Tab5 = new QLabel(("Наименование блока ВВТ"), tab5);
//    QLabel *labelc2Tab5 = new QLabel(("Код по классификатору. (Конкретные образцы оружия и военной техники)"), tab5);
//    QLabel *labelc3Tab5 = new QLabel(("Колличество по штату ВВТ"), tab5);
//    QLabel *labelc4Tab5 = new QLabel(("Колличество в наличии ВВТ"), tab5);
//    QLabel *labelc5Tab5 = new QLabel(("Укомплектованность ВВТ"), tab5);

//    textTab5Edit1->setReadOnly(true);
//    textTab5Edit2->setReadOnly(true);
//    textTab5Edit3->setReadOnly(true);
//    textTab5Edit4->setReadOnly(true);
//    textTab5Edit5->setReadOnly(true);

//    // устанавливаю layouts
//    tab5Layout->addWidget(labelc1Tab5);
//    tab5Layout->addWidget(textTab5Edit1);
//    tab5Layout->addWidget(labelc2Tab5);
//    tab5Layout->addWidget(textTab5Edit2);
//    tab5Layout->addWidget(labelc3Tab5);
//    tab5Layout->addWidget(textTab5Edit3);
//    tab5Layout->addWidget(labelc4Tab5);
//    tab5Layout->addWidget(textTab5Edit4);
//    tab5Layout->addWidget(labelc5Tab5);
//    tab5Layout->addWidget(textTab5Edit5);

//    tab5Layout->addStretch(1);

//    QVBoxLayout *tab6Layout = new QVBoxLayout(tab6);
//    textTab6Edit1 = new QTextEdit(tab6);
//    textTab6Edit1->setObjectName("TextEdit1Tab6");
//    textTab6Edit2 = new QTextEdit(tab6);
//    textTab6Edit2->setObjectName("TextEdit2Tab6");
//    textTab6Edit3 = new QTextEdit(tab6);
//    textTab6Edit3->setObjectName("TextEdit3Tab6");
//    textTab6Edit4 = new QTextEdit(tab6);
//    textTab6Edit4->setObjectName("TextEdit4Tab6");
//    textTab6Edit5 = new QTextEdit(tab6);
//    textTab6Edit5->setObjectName("TextEdit5Tab6");
//    textTab6Edit6 = new QTextEdit(tab6);
//    textTab6Edit6->setObjectName("TextEdit6Tab6");
//    textTab6Edit7 = new QTextEdit(tab6);
//    textTab6Edit7->setObjectName("TextEdit7Tab6");


//tab6->setLayout( tab6Layout );

//    QLabel *labelc1Tab6 = new QLabel(("Ключ знака ( уникальный идентификатор )"), tab6);
//    QLabel *labelc2Tab6 = new QLabel(("Код знака ( код серии знаков )"), tab6);
//    QLabel *labelc3Tab6 = new QLabel(("Тип знака ( векторный / линейный / площадной ) => VEC - векторный"), tab6);
//    QLabel *labelc4Tab6 = new QLabel(("Метрика знака. Координаты объекта ( 1я точка - расположение знака )"), tab6);
//    QLabel *labelc5Tab6 = new QLabel(("Метрика знака. Координаты объекта ( 2я точка - направление знака )"), tab6);
//    QLabel *labelc6Tab6 = new QLabel(("Семантика знака. Надпись внутри знака "), tab6);
//    QLabel *labelc7Tab6 = new QLabel(("Семантика знака. Надпись возле знака"), tab6);

//    textTab6Edit1->setReadOnly(true);
//    textTab6Edit2->setReadOnly(true);
//    textTab6Edit3->setReadOnly(true);
//    textTab6Edit4->setReadOnly(true);
//    textTab6Edit5->setReadOnly(true);
//    textTab6Edit6->setReadOnly(true);
//    textTab6Edit7->setReadOnly(true);


//    // устанавливаю layouts
//    tab6Layout->addWidget(labelc1Tab6);
//    tab6Layout->addWidget(textTab6Edit1);
//    tab6Layout->addWidget(labelc2Tab6);
//    tab6Layout->addWidget(textTab6Edit2);
//    tab6Layout->addWidget(labelc3Tab6);
//    tab6Layout->addWidget(textTab6Edit3);
//    tab6Layout->addWidget(labelc4Tab6);
//    tab6Layout->addWidget(textTab6Edit4);
//    tab6Layout->addWidget(labelc5Tab6);
//    tab6Layout->addWidget(textTab6Edit5);
//    tab6Layout->addWidget(labelc6Tab6);
//    tab6Layout->addWidget(textTab6Edit6);
//    tab6Layout->addWidget(labelc7Tab6);
//    tab6Layout->addWidget(textTab6Edit7);

//    tab6Layout->addStretch(1);


//}

void MainWindow::onHideEmptyClicked()
{
   bool hideEmptyFields = static_cast<QCheckBox*>(sender())->isChecked();
   if (tabWidget->currentIndex() == 0)
   {
       if (hideEmptyFields)
{
     if (textTab1Edit1->toPlainText().isEmpty())
     {
       textTab1Edit1->hide();
       labela1Tab1->hide();

     }
      if (textTab1Edit2->toPlainText().isEmpty())
      {
          textTab1Edit2->hide();
          labela2Tab1->hide();
   }
      if (textTab1Edit3->toPlainText().isEmpty())
      {
          textTab1Edit3->hide();
          labela2Tab1->hide();
   }
      if (textTab1Edit4->toPlainText().isEmpty())
      {
          textTab1Edit4->hide();
          labela3Tab1->hide();
   }
      if (textTab1Edit5->toPlainText().isEmpty())
      {
          textTab1Edit5->hide();
          labela4Tab1->hide();
   }
      if (textTab1Edit6->toPlainText().isEmpty())
      {
          textTab1Edit6->hide();
          labela5Tab1->hide();
   }
      if (textTab1Edit7->toPlainText().isEmpty())
      {
          textTab1Edit7->hide();
          labela6Tab1->hide();
   }
      if (textTab1Edit8->toPlainText().isEmpty())
      {
          textTab1Edit8->hide();
          labela7Tab1->hide();
   }
       }
      else
      {
          textTab1Edit1->show();
          labela1Tab1->show();
          textTab1Edit2->show();
          labela2Tab1->show();
          textTab1Edit3->show();
          labela3Tab1->show();
          textTab1Edit4->show();
          labela4Tab1->show();
          textTab1Edit5->show();
          labela5Tab1->show();
          textTab1Edit6->show();
          labela6Tab1->show();
          textTab1Edit7->show();
          labela7Tab1->show();
          textTab1Edit8->show();
          labela8Tab1->show();
      }
}

}

/// задаем модель данных
void MainWindow::setModel( VFVPModel &vfvpmodel )
{
    /// Учетный номер ВФВП
    vfvpmodel.charVfvp.vfvp_kod = "18200000000000040";
    /// Организационно-штатная принадлежность
    vfvpmodel.charVfvp.vfvp_org = "1";
    /// Дата и время состояния
    vfvpmodel.charVfvp.vfvp_dt  = QDateTime::currentDateTime();
    vfvpmodel.charVfvp.vfvp_name  = "101 ДШД";
//    vfvpmodel->charVfvp.vfvp_sname  =


    POSVFVP posVfvp; /// инициализируем модель данных положения воинского формирования
    /// наименование блока данных координат
    posVfvp.description = QString( "pvfvp" );
    /// геодезическая широта
    posVfvp.latitude    = QString( "56.314014573" );
    /// геодезическая долгота
    posVfvp.longitude   = QString( "42.911885365" );
    /// высота
    posVfvp.height      = QString( "404" );
    /// пояснение
    posVfvp.OK          = QString( "Позиция на высоте 404" );
    /// добавляем модель данных положения воинского формирования в коллекцию
    vfvpmodel.posVfvpLst.append( posVfvp );

    LSVFVP lsVfvp; /// инициализируем модель данных личного состава
    /// наименование блока личного состава
    lsVfvp.description = QString( "личный состав" );
    /// колличество по штату личного состава
    lsVfvp.kolshtat    = QString( "300" );
    /// колличество в наличии личного состава
    lsVfvp.kolnal      = QString( "30" );
    /// укомплектованность личного состава ( на самом деле 10, но так красивее ;-)
    lsVfvp.ukompl      = QString( "3" );
    /// добавляем модель данных личного состава в коллекцию
    vfvpmodel.lsVfvpLst.append( lsVfvp );

    VVTVFVP vvtVfvp; /// инициализируем модель данных ВВТ
    /// наименование блока ВВТ
    vvtVfvp.description = QString( "ВВТ" );
    /// код по классификатору "Конкретные образцы оружия и военной техники"
    vvtVfvp.kkoovt_kod  = QString( "5025200" ); /// М706(V-100) "КОММАНДО"
    /// колличество по штату ВВТ
    vvtVfvp.kolshtat    = QString( "300" );
    /// колличество в наличии ВВТ
    vvtVfvp.kolnal      = QString( "30" );
    /// укомплектованность ВВТ ( ну Вы поняли => 10 )
    vvtVfvp.ukompl      = QString( "3" );
    /// добавляем модель данных ВВТ в коллекцию
    vfvpmodel.vvtVfsvLst.append( vvtVfvp );

    /** пример файла данных ГИС векторный
     *****************************
     * .KEY V77708012            *
     * .COD 77708012 VEC         *
     * .MET 1                    *
     * 2                         *
     * 56.314014573 42.911885365 *
     * 56.962916333 44.038253903 *
     * .SEM 2                    *
     * 5400 10 дшд               *
     * 9216 КП                   *
     *****************************
    **/

    /// инициализируем модель картографических данных
    TXFDataWorker txfDataWorker;
    /// ключ знака ( уникальный идентификатор )
    txfDataWorker.setKEY( "V77708012" );
    /// код знака ( код серии знаков )
    txfDataWorker.setCOD( "77708012" );
    /// тип знака ( векторный / линейный / площадной ) => VEC - векторный
    txfDataWorker.setCOD_TYPE( "VEC" );
    /// метрика знака
    /// координаты объекта ( 1я точка - расположение знака )
    txfDataWorker.setMETValue( "56.314014573", "42.911885365" );
    /// координаты объекта ( 2я точка - направление знака )
    txfDataWorker.setMETValue( "56.962916333", "44.038253903" );
    /// семантика знака
    /// надпись внутри знака
    txfDataWorker.setSEMValue( "5400", "10 дшд" );
    /// надпись возле знака
    txfDataWorker.setSEMValue( "9216", "КП" );
    /// добавляем модель картографических данных в коллекцию
    vfvpmodel.txfDataLst.append( txfDataWorker );
    qDebug() << __PRETTY_FUNCTION__ << "txf count" << vfvpmodel.txfDataLst.count();
}

void MainWindow::setModel2(VFVPModel &vfvpmodel)
{
    /// Учетный номер ВФВП
    vfvpmodel.charVfvp.vfvp_kod = "18200000004000040";
    /// Организационно-штатная принадлежность
    vfvpmodel.charVfvp.vfvp_org = "18200000000000040";
    /// Дата и время состояния
    vfvpmodel.charVfvp.vfvp_dt  = QDateTime::currentDateTime();
    vfvpmodel.charVfvp.vfvp_name  = "29 ДШР";
}
void MainWindow::setModel3(VFVPModel &vfvpmodel)
{
    /// Учетный номер ВФВП
    vfvpmodel.charVfvp.vfvp_kod = "18200000004000788";
    /// Организационно-штатная принадлежность
    vfvpmodel.charVfvp.vfvp_org = "18200000004000040";
    /// Дата и время состояния
    vfvpmodel.charVfvp.vfvp_dt  = QDateTime::currentDateTime();
    vfvpmodel.charVfvp.vfvp_name  = "25 ДШР";
}
void MainWindow::setModel4(VFVPModel &vfvpmodel)
{
    /// Учетный номер ВФВП
    vfvpmodel.charVfvp.vfvp_kod = "18200000004000049";
    /// Организационно-штатная принадлежность
    vfvpmodel.charVfvp.vfvp_org = "18200000000000040";
    /// Дата и время состояния
    vfvpmodel.charVfvp.vfvp_dt  = QDateTime::currentDateTime();
    vfvpmodel.charVfvp.vfvp_name  = "16 ДШВ";
}
void MainWindow::onTreeViewItemClicked(const QModelIndex &modelIndex)
{   qDebug() << "Модель данных ТВФ в части ВФВП";

//    QString vfvp_kod ( modelIndex.data(RoleKodVf).toString() );
////        vfvpMap.value( vfvp_kod );
//    VFVPModel *vfvpmodel = vfvpMap.value( vfvp_kod );
//    qDebug() << vfvp_kod;
    QString vfvpKod = modelIndex.data(Qt::UserRole).toString();
    VFVPModel* vModelPtr = vfvpMap.value(vfvpKod);
    if (vModelPtr != nullptr) {
        vfvpmodel = *vModelPtr;
    } else {
        // Обработка случая, когда ключ не найден в мапе
        QMessageBox::warning(this, tr("Внимание!"), tr("Объект для данного ключа не найден! %1").arg(vfvpKod));
    }
    qDebug() << vfvpKod;

    /// характеристики ВФВП [ vfvp ]
    qDebug() << "характеристики ВФВП [ vfvp ]:";
    qDebug() << "vfvp_kod => " << vfvpmodel.charVfvp.vfvp_kod;
    qDebug() << "vfvp_org => " << vfvpmodel.charVfvp.vfvp_org;
    qDebug() << "vfvp_dt  => " << vfvpmodel.charVfvp.vfvp_dt;

    /// положение воинского формирования [ pvfvp ]
    qDebug() << "положение воинского формирования [ pvfvp ]:";
    qDebug() << "description => " << vfvpmodel.posVfvpLst.value( 0 ).description;
    qDebug() << "latitude    => " << vfvpmodel.posVfvpLst.value( 0 ).latitude;
    qDebug() << "longitude   => " << vfvpmodel.posVfvpLst.value( 0 ).longitude;
    qDebug() << "height      => " << vfvpmodel.posVfvpLst.value( 0 ).height;
    qDebug() << "OK          => " << vfvpmodel.posVfvpLst.value( 0 ).OK;

    /// личный состав [ lsvfvp ]
    qDebug() << "личный состав [ lsvfvp ]:";
    qDebug() << "description => " << vfvpmodel.lsVfvpLst.value( 0 ).description;
    qDebug() << "kolshtat    => " << vfvpmodel.lsVfvpLst.value( 0 ).kolshtat;
    qDebug() << "kolnal      => " << vfvpmodel.lsVfvpLst.value( 0 ).kolnal;
    qDebug() << "ukompl      => " << vfvpmodel.lsVfvpLst.value( 0 ).ukompl;

    /// ВВТ [ vvtvfvp ]
    qDebug() << "ВВТ [ vvtvfvp ]:";
    qDebug() << "description => " << vfvpmodel.vvtVfsvLst.value( 0 ).description;
    qDebug() << "kkoovt_kod  => " << vfvpmodel.vvtVfsvLst.value( 0 ).kkoovt_kod;
    qDebug() << "kolshtat    => " << vfvpmodel.vvtVfsvLst.value( 0 ).kolshtat;
    qDebug() << "kolnal      => " << vfvpmodel.vvtVfsvLst.value( 0 ).kolnal;
    qDebug() << "ukompl      => " << vfvpmodel.vvtVfsvLst.value( 0 ).ukompl;

    /// картографические данные [ kdou ]
    qDebug() << "картографические данные [ kdou ]:";
    if ( !vfvpmodel.txfDataLst.isEmpty() ) {
        TXFDataWorker txfDataWorker( vfvpmodel.txfDataLst.at( 0 ) );
        qDebug() << "KEY      => " << txfDataWorker.getKEY();
        qDebug() << "COD      => " << txfDataWorker.getCOD();
        qDebug() << "COD_TYPE => " << txfDataWorker.getCOD_TYPE();
        qDebug() << "MET 1    => " << txfDataWorker.getMET().at( 0 );
        qDebug() << "MET 2    => " << txfDataWorker.getMET().at( 1 );
        QString semKey( txfDataWorker.getSEMList().at( 0 ) );
        qDebug() << "SEM 1    => " << semKey << txfDataWorker.getSEMValue( semKey );
        semKey = txfDataWorker.getSEMList().at( 1 );
        qDebug() << "SEM 2    => " << semKey << txfDataWorker.getSEMValue( semKey );

        /// картографические данные [ kdou ] ( txf )
        qDebug() << "картографические данные [ kdou ] ( txf ):";
        qDebug() << txfDataWorker.createTxfData().toStdString().c_str();
    }


         qDebug() << __PRETTY_FUNCTION__ << modelIndex.data(Qt::UserRole).toString();
//        qDebug() << __PRETTY_FUNCTION__ << pTextEdit2->objectName();

//        pTextEdit1->setText( modelIndex.data().toString());
//        pTextEdit2->setText( "TextEdit 2 new" );
//  textTab1Edit1->setText( modelIndex.data().toString() );
//  textTab1Edit2->setText( modelIndex.data().toString() );
//  textTab1Edit3->setText( modelIndex.data().toString() );
//  textTab2Edit3->setText( modelIndex.data().toString() );
        qDebug() << "Модель данных ТВФ в части ВФВП";


//        for (int i = 0;  i < vfvpMap.size(); ++i )
//        {

            /// характеристики ВФВП [ vfvp ] => modelView
            textTab1Edit1->setText( vfvpmodel.charVfvp.vfvp_kod );
            textTab1Edit2->setText( vfvpmodel.charVfvp.vfvp_dlkod );
            textTab1Edit3->setText( vfvpmodel.charVfvp.vfvp_dt.toString() );
            textTab1Edit4->setText( vfvpmodel.charVfvp.vfvp_num );
            textTab1Edit5->setText( vfvpmodel.charVfvp.ktvfig_kod );
            textTab1Edit6->setText( vfvpmodel.charVfvp.kvvsig_kod );
            textTab1Edit7->setText( vfvpmodel.charVfvp.krvig_kod );
            textTab1Edit8->setText( vfvpmodel.charVfvp.vfvp_org );
            textTab1Edit9->setText( vfvpmodel.charVfvp.oksm_kod );
            textTab1Edit10->setText( vfvpmodel.charVfvp.ksmt_kod );
            textTab1Edit11->setText( vfvpmodel.charVfvp.kuvf_kod );
            textTab1Edit12->setText( vfvpmodel.charVfvp.ktor_kod );
            textTab1Edit13->setText( vfvpmodel.charVfvp.vfvp_name );
            textTab1Edit14->setText( vfvpmodel.charVfvp.vfvp_sname );
            textTab1Edit15->setText( vfvpmodel.charVfvp.vfvp_kod_esutz );
            textTab1Edit16->setText( vfvpmodel.charVfvp.vfvp_vro.toString() );
            textTab1Edit17->setText( vfvpmodel.charVfvp.vfvp_numc );
            textTab1Edit18->setText( vfvpmodel.charVfvp.vfvp_numcnew );
            textTab1Edit19->setText( vfvpmodel.charVfvp.ksuo_kod );
            textTab1Edit20->setText( vfvpmodel.charVfvp.kpo_kod );
            textTab1Edit21->setText( vfvpmodel.charVfvp.kio_kod );
            textTab1Edit22->setText( vfvpmodel.charVfvp.kdo_kod );
            textTab1Edit23->setText( vfvpmodel.charVfvp.kir_kod );
            textTab1Edit24->setText( vfvpmodel.charVfvp.vfvp_bp );
            textTab1Edit25->setText( vfvpmodel.charVfvp.vfvp_importance );
            textTab1Edit26->setText( vfvpmodel.charVfvp.vfvp_info );
            textTab1Edit27->setText( vfvpmodel.charVfvp.kod_khdvf );
            textTab1Edit28->setText( vfvpmodel.charVfvp.kod_ksbs );
            textTab1Edit29->setText( vfvpmodel.charVfvp.height );

            /// положение воинского формирования [ pvfvp ] => modelView
            textTab2Edit1->setText( vfvpmodel.posVfvpLst.value( 0 ).description );
            textTab2Edit2->setText( vfvpmodel.posVfvpLst.value( 0 ).latitude );
            textTab2Edit3->setText( vfvpmodel.posVfvpLst.value( 0 ).longitude);
            textTab2Edit4->setText( vfvpmodel.posVfvpLst.value( 0 ).height );
            textTab2Edit5->setText( vfvpmodel.posVfvpLst.value( 0 ).OK );

            /// личный состав [ lsvfvp ] => modelView
            textTab4Edit1->setText( vfvpmodel.lsVfvpLst.value( 0 ).description );
            textTab4Edit2->setText( vfvpmodel.lsVfvpLst.value( 0 ).kolshtat );
            textTab4Edit3->setText( vfvpmodel.lsVfvpLst.value( 0 ).kolnal );
            textTab4Edit4->setText( vfvpmodel.lsVfvpLst.value( 0 ).ukompl );

            /// ВВТ [ vvtvfvp ] => modelView
            textTab5Edit1->setText( vfvpmodel.vvtVfsvLst.value( 0 ).description );
            textTab5Edit2->setText( vfvpmodel.vvtVfsvLst.value( 0 ).kkoovt_kod );
            textTab5Edit3->setText( vfvpmodel.vvtVfsvLst.value( 0 ).kolshtat );
            textTab5Edit4->setText( vfvpmodel.vvtVfsvLst.value( 0 ).kolnal );
            textTab5Edit5->setText( vfvpmodel.vvtVfsvLst.value( 0 ).ukompl );

          // WARNING textTab5Edit1->clear();

            /// картографические данные [ kdou ] => modelView
            if ( !vfvpmodel.txfDataLst.isEmpty() ) {
                TXFDataWorker txfDataWorker( vfvpmodel.txfDataLst.at( 0 ) );
                textTab6Edit1->setText( txfDataWorker.getKEY() );
                textTab6Edit2->setText( txfDataWorker.getCOD() );
                textTab6Edit3->setText( txfDataWorker.getCOD_TYPE() );
               // QVector<QPair<QString, QString>> met = txfDataWorker.getMET();
//                QString text;
//                for (const QPair<QString, QString>& pair : met)
//                {
//                    text += pair.first + ": " + pair.second + "\n";
//                }
//                textTab5Edit5->setText(text);
                //        QString semValueMet = ( vfvpmodel->txfDataLst.value( 0 )->getMET().first());
                //        textTab5Edit4->setText("MET 1    => " + " " + semValueMet);
                //        textTab5Edit4->setText( vfvpmodel->txfDataLst.value( 0 )->getMET().first());
                textTab6Edit4->setText( txfDataWorker.getMET().at( 0 ).first + " : " + txfDataWorker.getMET().at( 0 ).second);
                textTab6Edit5->setText( txfDataWorker.getMET().at( 1 ).first  + " : " + txfDataWorker.getMET().at( 1 ).second);
                QString semKeyGui( txfDataWorker.getSEMList().first() );
                QString semValue = ( txfDataWorker.getSEMValue( semKeyGui ));
                textTab6Edit6->setText("SEM 1    => " + semKeyGui + " " + semValue);
                QString semKey( txfDataWorker.getSEMList().at( 0 ) );
                QString semValue1 = ( txfDataWorker.getSEMValue( semKey ));
                textTab6Edit7->setText("SEM 2    => " + semKey + " " + semValue1);

                /// картографические данные [ kdou ] ( txf ) => modelView
                //textTab6Edit1->setText( txfDataWorker.createTxfData().toStdString().c_str() );
            }

            //        }
}
QList < VFVPModel * > MainWindow::getVfvpModelLst() const
{
    return vfvpModelLst_orig;
}

void MainWindow::setVfvpModelLst()
{
    setModel ( vfvpModel1 );
    setModel2( vfvpModel2 );
    setModel3( vfvpModel3 );
    setModel4( vfvpModel4 );

    vfvpModelLst_orig << &vfvpModel1 << &vfvpModel2 << &vfvpModel3 << &vfvpModel4;
}









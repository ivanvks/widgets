void MainWindow::createTabWidget()
{
    tabWidget = new QTabWidget(this);
    QSettings settings("organization", "application");

    QStringList tabNames = {
        "Характеристики",
        "Положение",
        "Направление",
        "Ли",
        "В",
        "Ка данные"
    };

    // Создание вкладок
    tab1 = createScrollableTab(tabNames[0], settings, true);
    tab2 = createScrollableTab(tabNames[1], settings, false);
    // ... создание остальных вкладок ...

    // Добавление вкладок в QTabWidget
    tabWidget->addTab(tab1, tabNames[0]);
    tabWidget->addTab(tab2, tabNames[1]);
    // ... добавление остальных вкладок ...

    // ...
}

QWidget* MainWindow::createScrollableTab(const QString& tabName, QSettings& settings, bool readOnly)
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

    QStringList labelNames = settings.value(tabName + "/LabelNames").toStringList();
    QStringList fieldNames = settings.value(tabName + "/FieldNames").toStringList();

    for (int i = 0; i < labelNames.size(); ++i) {
        QLabel* label = new QLabel(labelNames[i], scrollWidget);
        QTextEdit* textEdit = new QTextEdit(scrollWidget);

        QString fieldName = fieldNames[i];
        textEdit->setObjectName(QString("TextEdit%1%2").arg(fieldName).arg(tabName));
        textEdit->setReadOnly(readOnly);

        scrollLayout->addWidget(label);
        scrollLayout->addWidget(textEdit);
    }

    scrollLayout->addStretch(1);

    return tabWidget;
}


void MainWindow::createTabWidget()
{
    tabWidget = new QTabWidget(this);
    QSettings settings("organization", "application");

    QStringList tabNames = {
        "Характеристики",
        "Положение",
        "Направление",
        "Ли",
        "В",
        "Ка данные"
    };

    // Создание вкладок
    tab1 = createScrollableTab(tabNames[0], settings, true);
    tab2 = createScrollableTab(tabNames[1], settings, false);
    // ... создание остальных вкладок ...

    // Добавление вкладок в QTabWidget
    tabWidget->addTab(tab1, tabNames[0]);
    tabWidget->addTab(tab2, tabNames[1]);
    // ... добавление остальных вкладок ...

    // ...

    // Сохранение данных перед закрытием приложения
    saveSettings(settings);
}

QWidget* MainWindow::createScrollableTab(const QString& tabName, QSettings& settings, bool readOnly)
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

    QStringList labelNames;
    QStringList fieldNames;

    // Получение сохраненных данных для текущей вкладки
    labelNames = settings.value(tabName + "/LabelNames").toStringList();
    fieldNames = settings.value(tabName + "/FieldNames").toStringList();

    // Если сохраненные данные отсутствуют, создаем новые списки
    if (labelNames.isEmpty() || fieldNames.isEmpty()) {
        labelNames = getDefaultLabelNames(tabName);
        fieldNames = getDefaultFieldNames(tabName);
    }

    for (int i = 0; i < labelNames.size(); ++i) {
        QLabel* label = new QLabel(labelNames[i], scrollWidget);
        QTextEdit* textEdit = new QTextEdit(scrollWidget);

        QString fieldName = fieldNames[i];
        textEdit->setObjectName(QString("TextEdit%1%2").arg(fieldName).arg(tabName));
        textEdit->setReadOnly(readOnly);

        scrollLayout->addWidget(label);
        scrollLayout->addWidget(textEdit);
    }

    scrollLayout->addStretch(1);

    return tabWidget;
}

void MainWindow::saveSettings(QSettings& settings)
{
    // Сохранение данных для каждой вкладки
    saveTabSettings(tab1, "Характеристики", settings);
    saveTabSettings(tab2, "Положение", settings);
    // ... сохранение данных для остальных вкладок ...
}

void MainWindow::saveTabSettings(QWidget* tab, const QString& tabName, QSettings& settings)
{
    // Получение списка меток и полей для текущей вкладки
    QStringList labelNames;
    QStringList fieldNames;
   

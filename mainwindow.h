#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>
#include <QTreeView>
#include <QStandardItemModel>
#include <QTextEdit>
#include <QWidget>
#include <QScrollArea>
#include <QMap>
#include <QtWidgets/QScrollBar>
#include "connectdatabase.h"
#include "configwindow.h"
#include "vfvpmodel/include/vfvpmodel.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void checkConfigFile();
    void createCustomMenu();
    bool hasParent(VFVPModel* vfvpModel);
    void createChildItems(QStandardItem* parentItem, QList<QStandardItem*>& topLevelItems);
    QStandardItem* createTreeItem(VFVPModel* vfvpModel);
    QList<VFVPModel *> getVfvpModelLst() const;
    void setVfvpModelLst();
protected:
    void closeEvent(QCloseEvent *event) override;
private:
int counter = 1; // переменная-счетчик
    void createTreeView();
    void createTabWidget();

    /**
     * @brief setModel задаем модель данных
     * @param vfvpmodel модель данных
     */
    void setModel( VFVPModel &vfvpmodel );
    void setModel2( VFVPModel &vfvpmodel );
    void setModel3( VFVPModel &vfvpmodel );
    void setModel4( VFVPModel &vfvpmodel );

private slots:
    void openNewWindow();
    void updateDatabaseConnectionStatus(bool connected);
    void updateConnection(); // Новый слот для сохранения настроек
    void closeWindowCloseDbConnection();
    void onTreeViewItemClicked(const QModelIndex &modelIndex);
    void onHideEmptyClicked();


private:
    QWidget* createScrollableTab(const QString& tabName, const QStringList& labelNames, bool readOnly);

    QPushButton *button;
    QLabel* connectionStatusIcon;
    ConnectDatabase *connectDatabase;
    ConfigWindow *configWindow;

    QTreeView *treeView;
       QScrollArea *scrollArea;
    QStandardItemModel *model;
    QTabWidget *tabWidget;
    QWidget *tab1, *tab2, *tab3, *tab4, *tab5, *tab6;
    QTextEdit *textTab1Edit1, *textTab1Edit2, *textTab1Edit3, *textTab1Edit4, *textTab1Edit5, *textTab1Edit6;
    QTextEdit *textTab1Edit7, *textTab1Edit8, *textTab1Edit9, *textTab1Edit10, *textTab1Edit11, *textTab1Edit12;
    QTextEdit *textTab1Edit13, *textTab1Edit14, *textTab1Edit15, *textTab1Edit16, *textTab1Edit17, *textTab1Edit18;
    QTextEdit *textTab1Edit19, *textTab1Edit20, *textTab1Edit21, *textTab1Edit22, *textTab1Edit23, *textTab1Edit24;
    QTextEdit *textTab1Edit25, *textTab1Edit26, *textTab1Edit27, *textTab1Edit28, *textTab1Edit29;
    QTextEdit *textTab2Edit1, *textTab2Edit2, *textTab2Edit3, *textTab2Edit4, *textTab2Edit5;
 QTextEdit *textEdit;
    QTextEdit *textTab4Edit1, *textTab4Edit2, *textTab4Edit3, *textTab4Edit4;
    QTextEdit *textTab5Edit1, *textTab5Edit2, *textTab5Edit3, *textTab5Edit4, *textTab5Edit5;
    QTextEdit *textTab6Edit1, *textTab6Edit2, *textTab6Edit3, *textTab6Edit4, *textTab6Edit5, *textTab6Edit6, *textTab6Edit7;
    QLineEdit *leTab1Edit1;
    // QTextEdit *textTab6Edit1, *textTab6Edit2;
    QScrollBar *verticalScrollBar;
     VFVPModel  vfvpModel, vfvpModel1, vfvpModel2, vfvpModel3, vfvpModel4, vfvpmodel;
    QMap <QString, VFVPModel*> vfvpMap;
    QLabel *labela1Tab1, *labela2Tab1, *labela3Tab1, *labela4Tab1, *labela5Tab1, *labela6Tab1, *labela7Tab1, *labela8Tab1;
    QLabel *labela9Tab1, *labela10Tab1, *labela11Tab1, *labela12Tab1, *labela13Tab1, *labela14Tab1, *labela15Tab1, *labela16Tab1;
    QLabel *labela17Tab1, *labela18Tab1, *labela19Tab1, *labela20Tab1, *labela21Tab1, *labela22Tab1, *labela23Tab1, *labela24Tab1;
    QLabel *labela25Tab1, *labela26Tab1, *labela27Tab1, *labela28Tab1, *labela29Tab1;
QStandardItem *childItem;
     QList < VFVPModel * > vfvpModelLst, vfvpModelLst_orig;

};
#endif // MAINWINDOW_H

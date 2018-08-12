/********************************************************************************
** Form generated from reading UI file 'MainGUI.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINGUI_H
#define UI_MAINGUI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>
#include "widget/ClickableListWidget.h"
#include "widget/MTextEdit.h"

QT_BEGIN_NAMESPACE

class Ui_MainGUI
{
public:
    QAction *optionsAction;
    QAction *exitAction;
    QAction *restartAction;
    QWidget *centralwidget;
    QPlainTextEdit *inputTextArea;
    ClickableListWidget *channelList;
    ClickableListWidget *friendList;
    ClickableListWidget *gameList;
    QLineEdit *titleLabel;
    QPushButton *refreshButton;
    MTextEdit *outputTextArea;
    QMenuBar *menuBar;
    QMenu *fileMenu;

    void setupUi(QMainWindow *MainGUI)
    {
        if (MainGUI->objectName().isEmpty())
            MainGUI->setObjectName(QString::fromUtf8("MainGUI"));
        MainGUI->resize(1087, 571);
        MainGUI->setMinimumSize(QSize(500, 200));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/GProxy.gif"), QSize(), QIcon::Normal, QIcon::Off);
        MainGUI->setWindowIcon(icon);
        optionsAction = new QAction(MainGUI);
        optionsAction->setObjectName(QString::fromUtf8("optionsAction"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/images/Tool.png"), QSize(), QIcon::Normal, QIcon::Off);
        optionsAction->setIcon(icon1);
        exitAction = new QAction(MainGUI);
        exitAction->setObjectName(QString::fromUtf8("exitAction"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/images/Cancel.png"), QSize(), QIcon::Normal, QIcon::Off);
        exitAction->setIcon(icon2);
        restartAction = new QAction(MainGUI);
        restartAction->setObjectName(QString::fromUtf8("restartAction"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/images/Refresh.png"), QSize(), QIcon::Normal, QIcon::Off);
        restartAction->setIcon(icon3);
        centralwidget = new QWidget(MainGUI);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        QPalette palette;
        QBrush brush(QColor(230, 230, 230, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Text, brush);
        QBrush brush1(QColor(0, 0, 0, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        QBrush brush2(QColor(120, 120, 120, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        centralwidget->setPalette(palette);
        QFont font;
        font.setFamily(QString::fromUtf8("Courier New"));
        font.setPointSize(14);
        font.setBold(true);
        font.setWeight(75);
        centralwidget->setFont(font);
        inputTextArea = new QPlainTextEdit(centralwidget);
        inputTextArea->setObjectName(QString::fromUtf8("inputTextArea"));
        inputTextArea->setGeometry(QRect(0, 521, 661, 31));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Courier New"));
        font1.setPointSize(10);
        inputTextArea->setFont(font1);
        inputTextArea->setStyleSheet(QString::fromUtf8(""));
        channelList = new ClickableListWidget(centralwidget);
        channelList->setObjectName(QString::fromUtf8("channelList"));
        channelList->setGeometry(QRect(900, 30, 181, 301));
        channelList->setContextMenuPolicy(Qt::CustomContextMenu);
        channelList->setEditTriggers(QAbstractItemView::NoEditTriggers);
        channelList->setSelectionMode(QAbstractItemView::SingleSelection);
        friendList = new ClickableListWidget(centralwidget);
        friendList->setObjectName(QString::fromUtf8("friendList"));
        friendList->setGeometry(QRect(900, 330, 181, 221));
        friendList->setContextMenuPolicy(Qt::CustomContextMenu);
        friendList->setEditTriggers(QAbstractItemView::NoEditTriggers);
        friendList->setSelectionMode(QAbstractItemView::NoSelection);
        gameList = new ClickableListWidget(centralwidget);
        gameList->setObjectName(QString::fromUtf8("gameList"));
        gameList->setGeometry(QRect(665, 30, 231, 521));
        QFont font2;
        font2.setBold(true);
        font2.setWeight(75);
        gameList->setFont(font2);
        gameList->setContextMenuPolicy(Qt::NoContextMenu);
        gameList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        gameList->setEditTriggers(QAbstractItemView::NoEditTriggers);
        gameList->setSelectionMode(QAbstractItemView::SingleSelection);
        titleLabel = new QLineEdit(centralwidget);
        titleLabel->setObjectName(QString::fromUtf8("titleLabel"));
        titleLabel->setGeometry(QRect(900, 0, 181, 31));
        QPalette palette1;
        palette1.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette1.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        titleLabel->setPalette(palette1);
        QFont font3;
        font3.setFamily(QString::fromUtf8("Courier New"));
        font3.setPointSize(12);
        font3.setBold(true);
        font3.setWeight(75);
        titleLabel->setFont(font3);
        titleLabel->setCursor(QCursor(Qt::BlankCursor));
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setReadOnly(true);
        refreshButton = new QPushButton(centralwidget);
        refreshButton->setObjectName(QString::fromUtf8("refreshButton"));
        refreshButton->setGeometry(QRect(670, 0, 221, 31));
        outputTextArea = new MTextEdit(centralwidget);
        outputTextArea->setObjectName(QString::fromUtf8("outputTextArea"));
        outputTextArea->setGeometry(QRect(0, 0, 661, 521));
        QFont font4;
        font4.setFamily(QString::fromUtf8("Arial"));
        font4.setPointSize(9);
        font4.setBold(false);
        font4.setWeight(50);
        outputTextArea->setFont(font4);
        outputTextArea->setAutoFillBackground(true);
        outputTextArea->setUndoRedoEnabled(true);
        outputTextArea->setReadOnly(true);
        outputTextArea->setTextInteractionFlags(Qt::TextBrowserInteraction);
        MainGUI->setCentralWidget(centralwidget);
        menuBar = new QMenuBar(MainGUI);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1087, 21));
        fileMenu = new QMenu(menuBar);
        fileMenu->setObjectName(QString::fromUtf8("fileMenu"));
        MainGUI->setMenuBar(menuBar);

        menuBar->addAction(fileMenu->menuAction());
        fileMenu->addAction(optionsAction);
        fileMenu->addSeparator();
        fileMenu->addAction(restartAction);
        fileMenu->addAction(exitAction);

        retranslateUi(MainGUI);
        QObject::connect(exitAction, SIGNAL(triggered()), MainGUI, SLOT(close()));
        QObject::connect(gameList, SIGNAL(clicked(QMouseEvent*)), MainGUI, SLOT(onGameListItemClicked(QMouseEvent*)));
        QObject::connect(titleLabel, SIGNAL(textChanged(QString)), MainGUI, SLOT(onTitleLabelTextChanged()));
        QObject::connect(channelList, SIGNAL(customContextMenuRequested(QPoint)), MainGUI, SLOT(onChannelContextMenu(QPoint)));
        QObject::connect(friendList, SIGNAL(customContextMenuRequested(QPoint)), MainGUI, SLOT(onFriendsContextMenu(QPoint)));
        QObject::connect(refreshButton, SIGNAL(clicked()), MainGUI, SLOT(onRefreshButtonClicked()));
        QObject::connect(channelList, SIGNAL(clicked(QMouseEvent*)), MainGUI, SLOT(onChannellistItemClicked(QMouseEvent*)));
        QObject::connect(friendList, SIGNAL(clicked(QMouseEvent*)), MainGUI, SLOT(onFriendlistItemClicked(QMouseEvent*)));
        QObject::connect(optionsAction, SIGNAL(triggered()), MainGUI, SLOT(showConfigDialog()));
        QObject::connect(restartAction, SIGNAL(triggered()), MainGUI, SLOT(onRestart()));

        QMetaObject::connectSlotsByName(MainGUI);
    } // setupUi

    void retranslateUi(QMainWindow *MainGUI)
    {
        MainGUI->setWindowTitle(QApplication::translate("MainGUI", "GProxy GhostGraz v2.3.3", 0, QApplication::UnicodeUTF8));
        optionsAction->setText(QApplication::translate("MainGUI", "Options", 0, QApplication::UnicodeUTF8));
        exitAction->setText(QApplication::translate("MainGUI", "Exit", 0, QApplication::UnicodeUTF8));
        restartAction->setText(QApplication::translate("MainGUI", "Restart", 0, QApplication::UnicodeUTF8));
        inputTextArea->setPlainText(QString());
        titleLabel->setText(QApplication::translate("MainGUI", "Channel", 0, QApplication::UnicodeUTF8));
        refreshButton->setText(QApplication::translate("MainGUI", "Refresh", 0, QApplication::UnicodeUTF8));
        outputTextArea->setHtml(QApplication::translate("MainGUI", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Arial'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
        fileMenu->setTitle(QApplication::translate("MainGUI", "File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainGUI: public Ui_MainGUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINGUI_H

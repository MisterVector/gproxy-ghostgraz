/********************************************************************************
** Form generated from reading UI file 'GhostGrazLogininformationDialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GHOSTGRAZLOGININFORMATIONDIALOG_H
#define UI_GHOSTGRAZLOGININFORMATIONDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_GhostGrazLogininformationDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *headerLabel;
    QLabel *usernameLabel;
    QLabel *passwordLabel;
    QLineEdit *usernameTextfield;
    QLineEdit *passwordTextfield;

    void setupUi(QDialog *GhostGrazLogininformationDialog)
    {
        if (GhostGrazLogininformationDialog->objectName().isEmpty())
            GhostGrazLogininformationDialog->setObjectName(QString::fromUtf8("GhostGrazLogininformationDialog"));
        GhostGrazLogininformationDialog->resize(350, 150);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(GhostGrazLogininformationDialog->sizePolicy().hasHeightForWidth());
        GhostGrazLogininformationDialog->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/Tool.png"), QSize(), QIcon::Normal, QIcon::Off);
        GhostGrazLogininformationDialog->setWindowIcon(icon);
        buttonBox = new QDialogButtonBox(GhostGrazLogininformationDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(0, 120, 350, 30));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        headerLabel = new QLabel(GhostGrazLogininformationDialog);
        headerLabel->setObjectName(QString::fromUtf8("headerLabel"));
        headerLabel->setGeometry(QRect(0, 0, 350, 30));
        QFont font;
        font.setFamily(QString::fromUtf8("Calibri"));
        font.setPointSize(16);
        font.setBold(true);
        font.setWeight(75);
        headerLabel->setFont(font);
        headerLabel->setAlignment(Qt::AlignCenter);
        usernameLabel = new QLabel(GhostGrazLogininformationDialog);
        usernameLabel->setObjectName(QString::fromUtf8("usernameLabel"));
        usernameLabel->setGeometry(QRect(10, 50, 160, 20));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Arial"));
        font1.setPointSize(12);
        font1.setBold(false);
        font1.setWeight(50);
        usernameLabel->setFont(font1);
        passwordLabel = new QLabel(GhostGrazLogininformationDialog);
        passwordLabel->setObjectName(QString::fromUtf8("passwordLabel"));
        passwordLabel->setGeometry(QRect(10, 90, 160, 20));
        QFont font2;
        font2.setFamily(QString::fromUtf8("Arial"));
        font2.setPointSize(12);
        passwordLabel->setFont(font2);
        usernameTextfield = new QLineEdit(GhostGrazLogininformationDialog);
        usernameTextfield->setObjectName(QString::fromUtf8("usernameTextfield"));
        usernameTextfield->setGeometry(QRect(180, 50, 160, 20));
        QFont font3;
        font3.setFamily(QString::fromUtf8("Arial"));
        font3.setPointSize(11);
        usernameTextfield->setFont(font3);
        passwordTextfield = new QLineEdit(GhostGrazLogininformationDialog);
        passwordTextfield->setObjectName(QString::fromUtf8("passwordTextfield"));
        passwordTextfield->setGeometry(QRect(180, 90, 160, 20));
        passwordTextfield->setFont(font3);
        passwordTextfield->setEchoMode(QLineEdit::Password);

        retranslateUi(GhostGrazLogininformationDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), GhostGrazLogininformationDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), GhostGrazLogininformationDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(GhostGrazLogininformationDialog);
    } // setupUi

    void retranslateUi(QDialog *GhostGrazLogininformationDialog)
    {
        GhostGrazLogininformationDialog->setWindowTitle(QApplication::translate("GhostGrazLogininformationDialog", "GhostGraz Login", 0, QApplication::UnicodeUTF8));
        headerLabel->setText(QApplication::translate("GhostGrazLogininformationDialog", "GhostGraz Login information", 0, QApplication::UnicodeUTF8));
        usernameLabel->setText(QApplication::translate("GhostGrazLogininformationDialog", "GhostGraz username:", 0, QApplication::UnicodeUTF8));
        passwordLabel->setText(QApplication::translate("GhostGrazLogininformationDialog", "GhostGraz password:", 0, QApplication::UnicodeUTF8));
        usernameTextfield->setText(QString());
        passwordTextfield->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class GhostGrazLogininformationDialog: public Ui_GhostGrazLogininformationDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GHOSTGRAZLOGININFORMATIONDIALOG_H

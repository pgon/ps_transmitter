/********************************************************************************
** Form generated from reading UI file 'frmmain.ui'
**
** Created by: Qt User Interface Compiler version 5.12.11
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FRMMAIN_H
#define UI_FRMMAIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "qwt_plot.h"

QT_BEGIN_NAMESPACE

class Ui_frmMain
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_2;
    QLabel *lblSensorStatus;
    QVBoxLayout *verticalLayout;
    QwtPlot *trendViewer;
    QLabel *label;
    QTextEdit *logger;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnSalir;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *frmMain)
    {
        if (frmMain->objectName().isEmpty())
            frmMain->setObjectName(QString::fromUtf8("frmMain"));
        frmMain->resize(676, 661);
        centralwidget = new QWidget(frmMain);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout_2 = new QVBoxLayout(centralwidget);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout->addWidget(label_2);

        lblSensorStatus = new QLabel(centralwidget);
        lblSensorStatus->setObjectName(QString::fromUtf8("lblSensorStatus"));
        lblSensorStatus->setMinimumSize(QSize(25, 25));
        lblSensorStatus->setMaximumSize(QSize(25, 25));
        lblSensorStatus->setPixmap(QPixmap(QString::fromUtf8(":/imagenes/images/led_r_1.png")));
        lblSensorStatus->setScaledContents(true);
        lblSensorStatus->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(lblSensorStatus);


        verticalLayout_2->addLayout(horizontalLayout);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        trendViewer = new QwtPlot(centralwidget);
        trendViewer->setObjectName(QString::fromUtf8("trendViewer"));

        verticalLayout->addWidget(trendViewer);

        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout->addWidget(label);

        logger = new QTextEdit(centralwidget);
        logger->setObjectName(QString::fromUtf8("logger"));
        QFont font;
        font.setPointSize(8);
        logger->setFont(font);

        verticalLayout->addWidget(logger);

        verticalLayout->setStretch(0, 1);

        verticalLayout_2->addLayout(verticalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        btnSalir = new QPushButton(centralwidget);
        btnSalir->setObjectName(QString::fromUtf8("btnSalir"));

        horizontalLayout_2->addWidget(btnSalir);


        verticalLayout_2->addLayout(horizontalLayout_2);

        verticalLayout_2->setStretch(1, 1);
        frmMain->setCentralWidget(centralwidget);
        menubar = new QMenuBar(frmMain);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 676, 22));
        frmMain->setMenuBar(menubar);
        statusbar = new QStatusBar(frmMain);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        frmMain->setStatusBar(statusbar);

        retranslateUi(frmMain);

        btnSalir->setDefault(true);


        QMetaObject::connectSlotsByName(frmMain);
    } // setupUi

    void retranslateUi(QMainWindow *frmMain)
    {
        frmMain->setWindowTitle(QApplication::translate("frmMain", "Visor de Trendings", nullptr));
        label_2->setText(QApplication::translate("frmMain", "Estado del Sensor", nullptr));
        lblSensorStatus->setText(QString());
        label->setText(QApplication::translate("frmMain", "Log de Actividad", nullptr));
        btnSalir->setText(QApplication::translate("frmMain", "&Salir", nullptr));
    } // retranslateUi

};

namespace Ui {
    class frmMain: public Ui_frmMain {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FRMMAIN_H

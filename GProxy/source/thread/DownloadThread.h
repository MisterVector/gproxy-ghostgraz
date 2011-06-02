#ifndef DOWNLOADTHREAD_H
#define	DOWNLOADTHREAD_H

#include "MainGUI.h"
#include "bnet.h"

#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QVector>
#include <QStringList>

class DownloadThread : public QThread {
    Q_OBJECT

public:
    DownloadThread(MainGUI *p_mainGUI);
    virtual ~DownloadThread();
    void refresh();
    void stop();

protected:
    void run();

private:
    bool stopped;
    MainGUI *mainGUI;
    QUrl url;
    QNetworkAccessManager *manager;
    QVector<QString> vBotorder;

    QVector<QStringList> sortGamelist(QVector<QStringList> vGamelist);

private slots:
    void downloadFinished(QNetworkReply *reply);

signals:
    void signal_clearGamelist();
    void signal_addGame(QString, QString, QString);
};

#endif	/* DOWNLOADTHREAD_H */

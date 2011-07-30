#ifndef CONFIG_H
#define	CONFIG_H

#include <QFile>
#include <QString>
#include <QStringList>
#include <QList>
#include <QPair>
#include <QColor>
#include <QFont>

class Config : public QObject
{
    Q_OBJECT

public:
    Config(QString config);
    virtual ~Config();
    int loadConfig();
    bool hasRequiredValues();

    QList<QString> getKeys();
    QList<QString> getValues();
    QString getString(const QString &key);
    int getInt(const QString &key);
    bool getBoolean(const QString &key);
    QColor getColor(const QString &key);
    QFont getFont(const QString &key);

    bool setString(const QString &key, const QString &value);
    bool setInt(const QString &key, const int &value);
    bool setBoolean(const QString &key, const bool &value);
    bool setColor(const QString &key, const QColor &color);
    bool setFont(const QString &key, const QFont &font);

    void commit();

signals:
    void configSaved();

private:
    QFile* configFile;
    QList<QString> keys;
    QList<QString> values;

    void addKeys();
    void addValues(QString content);
    void addDefaultValue(const QString &key);
};

#endif	/* CONFIG_H */


#ifndef MTEXTEDIT_H
#define	MTEXTEDIT_H

#include "..\gproxy.h"

#include <QTextEdit>
#include <QScrollBar>
#include <QList>

class MTextEdit : public QTextEdit
{
public:
    MTextEdit(QWidget* parent = 0);
    virtual ~MTextEdit();

    void setConfig(Config* config);
    void appendColoredMessage(const ColoredMessage& coloredMessage);
    void repaint(const QString& colorType, const QColor& color, const QString& username = "");

private:
    Config* config;
    QList<ColoredMessage> coloredMessages;
    bool doLineBreak;

    QColor getColor(const ColoredMessage& coloredMessage);
    void resetVariables();
};

#endif	/* MTEXTEDIT_H */


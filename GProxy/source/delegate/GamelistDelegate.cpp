#include "GamelistDelegate.h"

void GamelistDelegate::paint (QPainter * painter,
        const QStyleOptionViewItem & option, const QModelIndex & index)
const
{
    QString bot = "Botname: " + index.data(GamelistDelegate::BOTNAME).toString();
    QString gamename = index.data(GamelistDelegate::GAMENAME).toString();
    int openSlotsCount = index.data(GamelistDelegate::OPEN_SLOTS).toInt();
    QString openSlots = "Open slots: " + QString::number(openSlotsCount);
    
    QBrush brush = getBrush(openSlotsCount);

    if (gamename.startsWith("in channel") || gamename == "-")
    {
        brush = QBrush(QColor(230, 230, 230));
    }

    QRect rect = option.rect;
    painter->save();

    painter->setPen(QColor(0, 0, 0));

    painter->fillRect(rect, brush);
    painter->drawLine(rect.left(), rect.bottom(), rect.right(), rect.bottom());

    painter->drawText(rect, Qt::AlignHCenter, gamename);
    rect.adjust(0, 15, 0, 0);

    if (rect.height() == 35)
    {
        painter->drawText(rect, Qt::AlignHCenter, openSlots);
        rect.adjust(0, 15, 0, 0);
        painter->drawText(rect, Qt::AlignHCenter, bot);
    }
    else
    {
        painter->drawText(rect, Qt::AlignLeft, openSlots);
        painter->drawText(rect, Qt::AlignRight, bot);
    }

    painter->restore();
}

QSize GamelistDelegate::sizeHint (const QStyleOptionViewItem & option,
        const QModelIndex & index)
const
{
    QString bot = "Botname: " + index.data(GamelistDelegate::BOTNAME).toString();
    QString openSlots = "Open slots: " + index.data(GamelistDelegate::OPEN_SLOTS).toString();

    if (QFontMetrics(QFont("Segoe UI", 9, QFont::Bold)).boundingRect(openSlots + "  " + bot).width() > option.rect.width())
    {
        return QSize(200, 50);
    }
    else
    {
        return QSize(200, 34);
    }
}

QBrush GamelistDelegate::getBrush (int openSlots) const
{
    if (openSlots >= 6)
    {
        return QBrush(QColor(102, 255, 153));
    }
    else if (openSlots >= 4)
    {
        return QBrush(QColor(255, 165, 0));
    }
    else if (openSlots >= 1)
    {
        return QBrush(QColor(255, 0, 0));
    }
    else if (openSlots == 0)
    {
        return QBrush(QColor(153, 0, 0));
    }
    else
    {
        return QBrush(QColor(230, 230, 230));
    }
}
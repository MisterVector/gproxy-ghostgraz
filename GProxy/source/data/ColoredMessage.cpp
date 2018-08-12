#include "coloredmessage.h"

ColoredMessage::ColoredMessage () { }

ColoredMessage::ColoredMessage(const QString& message, const ColorType& colorType, const QColor& color)
{
    setMessage(message);
    setColorType(colorType);
    setColor(color);
}

ColoredMessage::ColoredMessage (const ColoredMessage& orig)
{
    setMessage(orig.getMessage());
    setColorType(orig.getColorType());
    setColor(orig.getColor());
}

ColoredMessage& ColoredMessage::operator= (const ColoredMessage& orig)
{
    if (this == &orig)
    {
        return *this;
    }

    setMessage(orig.getMessage());
    setColorType(orig.getColorType());
    setColor(orig.getColor());

    return *this;
}

bool ColoredMessage::operator== (const ColoredMessage& coloredMessage)
{
    if (getMessage() == coloredMessage.getMessage()
            && getColorType() == coloredMessage.getColorType()
            && getColor() == coloredMessage.getColor())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool ColoredMessage::operator!= (const ColoredMessage& coloredMessage)
{
    if (getMessage() == coloredMessage.getMessage()
            && getColorType() == coloredMessage.getColorType()
            && getColor() == coloredMessage.getColor())
    {
        return false;
    }
    else
    {
        return true;
    }
}

ColoredMessage::~ColoredMessage () { }

// <editor-fold defaultstate="collapsed" desc="Getters and setters">
QColor ColoredMessage::getColor() const
{
    return color;
}

void ColoredMessage::setColor(const QColor& color)
{
    this->color = color;
}

ColoredMessage::ColorType ColoredMessage::getColorType() const
{
    return colorType;
}

void ColoredMessage::setColorType(const ColorType& colorType)
{
    this->colorType = colorType;
}

QString ColoredMessage::getMessage() const
{
    return message;
}

void ColoredMessage::setMessage(const QString& message)
{
    this->message = message;
}
// </editor-fold>

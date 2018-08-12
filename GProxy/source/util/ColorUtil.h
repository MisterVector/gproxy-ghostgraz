#ifndef COLORUTIL_H
#define	COLORUTIL_H

#include "..\data\ColoredMessage.h"

class ColorUtil
{
public:
    static QString toString(const ColoredMessage::ColorType& colorType);
    static void validateUserColor(ColoredMessage& coloredMessage);

private:
    ColorUtil();
};

#endif	/* COLORUTIL_H */


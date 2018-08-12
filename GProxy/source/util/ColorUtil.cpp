#include "ColorUtil.h"

#include <QMetaObject>
#include <QMetaEnum>

/**
 * Hidden constructor.
 * Utility pattern.
 */
ColorUtil::ColorUtil () { }

QString ColorUtil::toString(const ColoredMessage::ColorType& colorType)
{
    QMetaObject metaObject = ColoredMessage::staticMetaObject;
    for (int i = 0; i < metaObject.enumeratorCount(); ++i)
    {
        QMetaEnum metaEnum = metaObject.enumerator(i);
        if (metaEnum.name() == QLatin1String("ColorType"))
        {
            return QLatin1String(metaEnum.valueToKey(colorType));
        }
    }

    // This should never be possible!
    return "default";
}
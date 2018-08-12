/****************************************************************************
** Meta object code from reading C++ file 'ColoredMessage.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "data/ColoredMessage.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ColoredMessage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ColoredMessage[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       1,   14, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // enums: name, flags, count, data
      15, 0x0,   11,   18,

 // enum data: key, value
      25, uint(ColoredMessage::DEFAULT),
      33, uint(ColoredMessage::WHISPER),
      41, uint(ColoredMessage::GAMEINFO),
      50, uint(ColoredMessage::GPROXY),
      57, uint(ColoredMessage::BNET),
      62, uint(ColoredMessage::INFO),
      67, uint(ColoredMessage::EMOTE),
      73, uint(ColoredMessage::WARNING),
      81, uint(ColoredMessage::ERROR),
      87, uint(ColoredMessage::USERCOLOR),
      97, uint(ColoredMessage::CUSTOM),

       0        // eod
};

static const char qt_meta_stringdata_ColoredMessage[] = {
    "ColoredMessage\0ColorType\0DEFAULT\0"
    "WHISPER\0GAMEINFO\0GPROXY\0BNET\0INFO\0"
    "EMOTE\0WARNING\0ERROR\0USERCOLOR\0CUSTOM\0"
};

void ColoredMessage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ColoredMessage::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ColoredMessage::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ColoredMessage,
      qt_meta_data_ColoredMessage, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ColoredMessage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ColoredMessage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ColoredMessage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ColoredMessage))
        return static_cast<void*>(const_cast< ColoredMessage*>(this));
    return QObject::qt_metacast(_clname);
}

int ColoredMessage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE

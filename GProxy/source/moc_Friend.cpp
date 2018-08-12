/****************************************************************************
** Meta object code from reading C++ file 'Friend.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "data/Friend.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Friend.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Friend[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       2,   14, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // enums: name, flags, count, data
       7, 0x0,    6,   22,
      16, 0x0,    8,   34,

 // enum data: key, value
      23, uint(Friend::NOT_AVAILABLE),
      37, uint(Friend::NOT_IN_CHANNEL),
      52, uint(Friend::IN_CHANNEL),
      63, uint(Friend::IN_PUBLIC_GAME),
      78, uint(Friend::IN_UNKNOWN_PRIVATE_GAME),
     102, uint(Friend::IN_PRIVATE_GAME),
     118, uint(Friend::OFFLINE),
     126, uint(Friend::MUTUAL),
     133, uint(Friend::DND),
     137, uint(Friend::MUTUAL_DND),
     148, uint(Friend::AWAY),
     153, uint(Friend::MUTUAL_AWAY),
     165, uint(Friend::DND_AWAY),
     174, uint(Friend::MUTUAL_DND_AWAY),

       0        // eod
};

static const char qt_meta_stringdata_Friend[] = {
    "Friend\0Location\0Status\0NOT_AVAILABLE\0"
    "NOT_IN_CHANNEL\0IN_CHANNEL\0IN_PUBLIC_GAME\0"
    "IN_UNKNOWN_PRIVATE_GAME\0IN_PRIVATE_GAME\0"
    "OFFLINE\0MUTUAL\0DND\0MUTUAL_DND\0AWAY\0"
    "MUTUAL_AWAY\0DND_AWAY\0MUTUAL_DND_AWAY\0"
};

void Friend::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Friend::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Friend::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Friend,
      qt_meta_data_Friend, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Friend::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Friend::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Friend::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Friend))
        return static_cast<void*>(const_cast< Friend*>(this));
    return QObject::qt_metacast(_clname);
}

int Friend::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE

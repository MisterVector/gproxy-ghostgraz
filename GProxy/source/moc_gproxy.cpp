/****************************************************************************
** Meta object code from reading C++ file 'gproxy.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gproxy.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gproxy.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CGProxy[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      16,       // signalCount

 // signals: signature, parameters, type, tag, flags
       9,    8,    8,    8, 0x05,
      40,   36,    8,    8, 0x05,
      89,    8,    8,    8, 0x05,
     121,  119,    8,    8, 0x05,
     160,    8,    8,    8, 0x05,
     194,    8,    8,    8, 0x05,
     234,    8,    8,    8, 0x05,
     263,    8,    8,    8, 0x05,
     289,    8,    8,    8, 0x05,
     324,    8,    8,    8, 0x05,
     358,    8,    8,    8, 0x05,
     391,    8,    8,    8, 0x05,
     427,    8,    8,    8, 0x05,
     455,    8,    8,    8, 0x05,
     485,    8,    8,    8, 0x05,
     513,    8,    8,    8, 0x05,

 // slots: signature, parameters, type, tag, flags
     534,    8,    8,    8, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CGProxy[] = {
    "CGProxy\0\0signal_startUpdateThread()\0"
    ",,,\0signal_addMessage(ColoredMessage,bool,bool,bool)\0"
    "signal_changeChannel(QString)\0,\0"
    "signal_addChannelUser(QString,QString)\0"
    "signal_removeChannelUser(QString)\0"
    "signal_updateFriendlist(QList<Friend*>)\0"
    "signal_updateFriend(Friend*)\0"
    "signal_addFriend(Friend*)\0"
    "signal_removeFriend(unsigned char)\0"
    "signal_setGameslots(QList<Slot*>)\0"
    "signal_showErrorMessage(QString)\0"
    "signal_playerJoined(ColoredMessage)\0"
    "signal_stopDownloadThread()\0"
    "signal_showConfigDialog(bool)\0"
    "signal_initConfigurations()\0"
    "signal_applyConfig()\0applyConfig()\0"
};

void CGProxy::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CGProxy *_t = static_cast<CGProxy *>(_o);
        switch (_id) {
        case 0: _t->signal_startUpdateThread(); break;
        case 1: _t->signal_addMessage((*reinterpret_cast< ColoredMessage(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 2: _t->signal_changeChannel((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->signal_addChannelUser((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 4: _t->signal_removeChannelUser((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->signal_updateFriendlist((*reinterpret_cast< QList<Friend*>(*)>(_a[1]))); break;
        case 6: _t->signal_updateFriend((*reinterpret_cast< Friend*(*)>(_a[1]))); break;
        case 7: _t->signal_addFriend((*reinterpret_cast< Friend*(*)>(_a[1]))); break;
        case 8: _t->signal_removeFriend((*reinterpret_cast< unsigned char(*)>(_a[1]))); break;
        case 9: _t->signal_setGameslots((*reinterpret_cast< QList<Slot*>(*)>(_a[1]))); break;
        case 10: _t->signal_showErrorMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 11: _t->signal_playerJoined((*reinterpret_cast< const ColoredMessage(*)>(_a[1]))); break;
        case 12: _t->signal_stopDownloadThread(); break;
        case 13: _t->signal_showConfigDialog((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 14: _t->signal_initConfigurations(); break;
        case 15: _t->signal_applyConfig(); break;
        case 16: _t->applyConfig(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData CGProxy::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CGProxy::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CGProxy,
      qt_meta_data_CGProxy, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CGProxy::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CGProxy::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CGProxy::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CGProxy))
        return static_cast<void*>(const_cast< CGProxy*>(this));
    return QObject::qt_metacast(_clname);
}

int CGProxy::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    }
    return _id;
}

// SIGNAL 0
void CGProxy::signal_startUpdateThread()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void CGProxy::signal_addMessage(ColoredMessage _t1, bool _t2, bool _t3, bool _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CGProxy::signal_changeChannel(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void CGProxy::signal_addChannelUser(QString _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void CGProxy::signal_removeChannelUser(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void CGProxy::signal_updateFriendlist(QList<Friend*> _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void CGProxy::signal_updateFriend(Friend * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void CGProxy::signal_addFriend(Friend * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void CGProxy::signal_removeFriend(unsigned char _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void CGProxy::signal_setGameslots(QList<Slot*> _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void CGProxy::signal_showErrorMessage(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void CGProxy::signal_playerJoined(const ColoredMessage & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void CGProxy::signal_stopDownloadThread()
{
    QMetaObject::activate(this, &staticMetaObject, 12, 0);
}

// SIGNAL 13
void CGProxy::signal_showConfigDialog(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void CGProxy::signal_initConfigurations()
{
    QMetaObject::activate(this, &staticMetaObject, 14, 0);
}

// SIGNAL 15
void CGProxy::signal_applyConfig()
{
    QMetaObject::activate(this, &staticMetaObject, 15, 0);
}
QT_END_MOC_NAMESPACE

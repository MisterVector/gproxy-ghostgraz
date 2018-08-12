/****************************************************************************
** Meta object code from reading C++ file 'GameListDownloader.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "GameListDownloader.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GameListDownloader.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GameListDownloader[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      29,   20,   19,   19, 0x05,

 // slots: signature, parameters, type, tag, flags
      68,   19,   19,   19, 0x0a,
      90,   84,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_GameListDownloader[] = {
    "GameListDownloader\0\0gameList\0"
    "downloadFinished(QList<GameListEntry>)\0"
    "startDownload()\0reply\0"
    "onDownloadFinished(QNetworkReply*)\0"
};

void GameListDownloader::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        GameListDownloader *_t = static_cast<GameListDownloader *>(_o);
        switch (_id) {
        case 0: _t->downloadFinished((*reinterpret_cast< QList<GameListEntry>(*)>(_a[1]))); break;
        case 1: _t->startDownload(); break;
        case 2: _t->onDownloadFinished((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData GameListDownloader::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GameListDownloader::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_GameListDownloader,
      qt_meta_data_GameListDownloader, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GameListDownloader::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GameListDownloader::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GameListDownloader::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GameListDownloader))
        return static_cast<void*>(const_cast< GameListDownloader*>(this));
    return QObject::qt_metacast(_clname);
}

int GameListDownloader::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void GameListDownloader::downloadFinished(QList<GameListEntry> _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE

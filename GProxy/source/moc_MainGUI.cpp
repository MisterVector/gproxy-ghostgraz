/****************************************************************************
** Meta object code from reading C++ file 'MainGUI.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "MainGUI.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainGUI.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainGUI[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      37,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
       9,    8,    8,    8, 0x0a,
      73,   29,    8,    8, 0x0a,
     149,  115,    8,    8, 0x2a,
     205,  186,    8,    8, 0x2a,
     252,  237,    8,    8, 0x2a,
     287,  279,    8,    8, 0x0a,
     327,  310,    8,    8, 0x0a,
     368,  359,    8,    8, 0x0a,
     403,  395,    8,    8, 0x0a,
     438,  436,    8,    8, 0x0a,
     460,  436,    8,    8, 0x0a,
     491,  479,    8,    8, 0x0a,
     528,  519,    8,    8, 0x0a,
     568,  555,    8,    8, 0x0a,
     607,  594,    8,    8, 0x0a,
     630,    8,    8,    8, 0x2a,
     660,  649,    8,    8, 0x0a,
     689,    8,    8,    8, 0x0a,
     710,    8,    8,    8, 0x0a,
     735,  724,    8,    8, 0x0a,
     770,  760,    8,    8, 0x0a,
     793,    8,    8,    8, 0x08,
     803,    8,    8,    8, 0x08,
     815,    8,    8,    8, 0x08,
     841,    8,    8,    8, 0x08,
     870,    8,    8,    8, 0x08,
     899,    8,    8,    8, 0x08,
     935,    8,    8,    8, 0x08,
     960,    8,    8,    8, 0x08,
     982,    8,    8,    8, 0x08,
     998,    8,    8,    8, 0x08,
    1037,    8,    8,    8, 0x08,
    1075,    8,    8,    8, 0x08,
    1100,    8,    8,    8, 0x08,
    1135,    8,    8,    8, 0x08,
    1180, 1168,    8,    8, 0x08,
    1221, 1212,    8,    8, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainGUI[] = {
    "MainGUI\0\0startUpdateThread()\0"
    "coloredMessage,log,printTimestamp,lineBreak\0"
    "addMessage(ColoredMessage,bool,bool,bool)\0"
    "coloredMessage,log,printTimestamp\0"
    "addMessage(ColoredMessage,bool,bool)\0"
    "coloredMessage,log\0addMessage(ColoredMessage,bool)\0"
    "coloredMessage\0addMessage(ColoredMessage)\0"
    "channel\0changeChannel(QString)\0"
    "username,clanTag\0addChannelUser(QString,QString)\0"
    "username\0removeChannelUser(QString)\0"
    "friends\0updateFriendlist(QList<Friend*>)\0"
    "f\0updateFriend(Friend*)\0addFriend(Friend*)\0"
    "entryNumber\0removeFriend(unsigned char)\0"
    "slotList\0setGameslots(QList<Slot*>)\0"
    "errorMessage\0showErrorMessage(QString)\0"
    "exitOnReject\0showConfigDialog(bool)\0"
    "showConfigDialog()\0playername\0"
    "playerJoined(ColoredMessage)\0"
    "initConfigurations()\0applyConfig()\0"
    "area,color\0setColor(QString,QColor)\0"
    "area,font\0setFont(QString,QFont)\0"
    "onClose()\0onRestart()\0onTitleLabelTextChanged()\0"
    "onChannelContextMenu(QPoint)\0"
    "onFriendsContextMenu(QPoint)\0"
    "onGameListItemClicked(QMouseEvent*)\0"
    "onRefreshButtonClicked()\0updateRefreshButton()\0"
    "startWarcraft()\0onChannellistItemClicked(QMouseEvent*)\0"
    "onFriendlistItemClicked(QMouseEvent*)\0"
    "statspageLoginFinished()\0"
    "receivedPlayerInformation(Player*)\0"
    "onAdminlistReceived(QStringList)\0"
    "colorDialog\0userColorChanged(MColorDialog*)\0"
    "gameList\0onGameListDownloadFinished(QList<GameListEntry>)\0"
};

void MainGUI::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MainGUI *_t = static_cast<MainGUI *>(_o);
        switch (_id) {
        case 0: _t->startUpdateThread(); break;
        case 1: _t->addMessage((*reinterpret_cast< const ColoredMessage(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 2: _t->addMessage((*reinterpret_cast< const ColoredMessage(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 3: _t->addMessage((*reinterpret_cast< const ColoredMessage(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 4: _t->addMessage((*reinterpret_cast< const ColoredMessage(*)>(_a[1]))); break;
        case 5: _t->changeChannel((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: _t->addChannelUser((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 7: _t->removeChannelUser((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 8: _t->updateFriendlist((*reinterpret_cast< QList<Friend*>(*)>(_a[1]))); break;
        case 9: _t->updateFriend((*reinterpret_cast< Friend*(*)>(_a[1]))); break;
        case 10: _t->addFriend((*reinterpret_cast< Friend*(*)>(_a[1]))); break;
        case 11: _t->removeFriend((*reinterpret_cast< unsigned char(*)>(_a[1]))); break;
        case 12: _t->setGameslots((*reinterpret_cast< QList<Slot*>(*)>(_a[1]))); break;
        case 13: _t->showErrorMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 14: _t->showConfigDialog((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 15: _t->showConfigDialog(); break;
        case 16: _t->playerJoined((*reinterpret_cast< const ColoredMessage(*)>(_a[1]))); break;
        case 17: _t->initConfigurations(); break;
        case 18: _t->applyConfig(); break;
        case 19: _t->setColor((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QColor(*)>(_a[2]))); break;
        case 20: _t->setFont((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QFont(*)>(_a[2]))); break;
        case 21: _t->onClose(); break;
        case 22: _t->onRestart(); break;
        case 23: _t->onTitleLabelTextChanged(); break;
        case 24: _t->onChannelContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 25: _t->onFriendsContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 26: _t->onGameListItemClicked((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 27: _t->onRefreshButtonClicked(); break;
        case 28: _t->updateRefreshButton(); break;
        case 29: _t->startWarcraft(); break;
        case 30: _t->onChannellistItemClicked((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 31: _t->onFriendlistItemClicked((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 32: _t->statspageLoginFinished(); break;
        case 33: _t->receivedPlayerInformation((*reinterpret_cast< Player*(*)>(_a[1]))); break;
        case 34: _t->onAdminlistReceived((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 35: _t->userColorChanged((*reinterpret_cast< MColorDialog*(*)>(_a[1]))); break;
        case 36: _t->onGameListDownloadFinished((*reinterpret_cast< QList<GameListEntry>(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MainGUI::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MainGUI::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainGUI,
      qt_meta_data_MainGUI, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainGUI::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainGUI::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainGUI::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainGUI))
        return static_cast<void*>(const_cast< MainGUI*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainGUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 37)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 37;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

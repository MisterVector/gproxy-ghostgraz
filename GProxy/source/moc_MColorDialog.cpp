/****************************************************************************
** Meta object code from reading C++ file 'MColorDialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "widget/MColorDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MColorDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MColorDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      26,   14,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      61,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MColorDialog[] = {
    "MColorDialog\0\0colorDialog\0"
    "currentColorChanged(MColorDialog*)\0"
    "onCurrentColorChanged(QColor)\0"
};

void MColorDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MColorDialog *_t = static_cast<MColorDialog *>(_o);
        switch (_id) {
        case 0: _t->currentColorChanged((*reinterpret_cast< MColorDialog*(*)>(_a[1]))); break;
        case 1: _t->onCurrentColorChanged((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MColorDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MColorDialog::staticMetaObject = {
    { &QColorDialog::staticMetaObject, qt_meta_stringdata_MColorDialog,
      qt_meta_data_MColorDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MColorDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MColorDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MColorDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MColorDialog))
        return static_cast<void*>(const_cast< MColorDialog*>(this));
    return QColorDialog::qt_metacast(_clname);
}

int MColorDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QColorDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void MColorDialog::currentColorChanged(MColorDialog * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE

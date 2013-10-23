/****************************************************************************
** Meta object code from reading C++ file 'GDOSoptions.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/gui/GDOSoptions.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GDOSoptions.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GDosoptions[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x0a,
      23,   12,   12,   12, 0x0a,
      43,   37,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GDosoptions[] = {
    "GDosoptions\0\0slot_OK()\0slot_Cancel()\0"
    "index\0slot_changeinterpol(int)\0"
};

void GDosoptions::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        GDosoptions *_t = static_cast<GDosoptions *>(_o);
        switch (_id) {
        case 0: _t->slot_OK(); break;
        case 1: _t->slot_Cancel(); break;
        case 2: _t->slot_changeinterpol((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData GDosoptions::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GDosoptions::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_GDosoptions,
      qt_meta_data_GDosoptions, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GDosoptions::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GDosoptions::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GDosoptions::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GDosoptions))
        return static_cast<void*>(const_cast< GDosoptions*>(this));
    return QDialog::qt_metacast(_clname);
}

int GDosoptions::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

/****************************************************************************
** Meta object code from reading C++ file 'Drudeoptions.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/gui/Drudeoptions.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Drudeoptions.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Drudeoptions[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x0a,
      24,   13,   13,   13, 0x0a,
      40,   38,   13,   13, 0x0a,
      61,   38,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Drudeoptions[] = {
    "Drudeoptions\0\0slot_OK()\0slot_Cancel()\0"
    "b\0slot_dokroeger(bool)\0slot_dorel(bool)\0"
};

void Drudeoptions::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Drudeoptions *_t = static_cast<Drudeoptions *>(_o);
        switch (_id) {
        case 0: _t->slot_OK(); break;
        case 1: _t->slot_Cancel(); break;
        case 2: _t->slot_dokroeger((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->slot_dorel((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Drudeoptions::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Drudeoptions::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Drudeoptions,
      qt_meta_data_Drudeoptions, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Drudeoptions::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Drudeoptions::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Drudeoptions::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Drudeoptions))
        return static_cast<void*>(const_cast< Drudeoptions*>(this));
    return QDialog::qt_metacast(_clname);
}

int Drudeoptions::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

/****************************************************************************
** Meta object code from reading C++ file 'interactive_parameter.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/gui/interactive_parameter.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'interactive_parameter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Interactive_Parameter[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x0a,
      56,   22,   22,   22, 0x0a,
     100,   89,   22,   22, 0x0a,
     124,   22,   22,   22, 0x0a,
     149,   22,   22,   22, 0x0a,
     181,  175,   22,   22, 0x0a,
     209,  175,   22,   22, 0x0a,
     237,   22,   22,   22, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Interactive_Parameter[] = {
    "Interactive_Parameter\0\0"
    "slot_change_upper_bound(QString)\0"
    "slot_change_lower_bound(QString)\0"
    "boundstate\0slot_change_bound(bool)\0"
    "slot_change_val(QString)\0"
    "slot_change_name(QString)\0state\0"
    "slot_change_displayed(bool)\0"
    "slot_change_canchange(bool)\0slot_ok()\0"
};

void Interactive_Parameter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Interactive_Parameter *_t = static_cast<Interactive_Parameter *>(_o);
        switch (_id) {
        case 0: _t->slot_change_upper_bound((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->slot_change_lower_bound((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->slot_change_bound((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->slot_change_val((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->slot_change_name((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->slot_change_displayed((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->slot_change_canchange((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->slot_ok(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Interactive_Parameter::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Interactive_Parameter::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Interactive_Parameter,
      qt_meta_data_Interactive_Parameter, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Interactive_Parameter::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Interactive_Parameter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Interactive_Parameter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Interactive_Parameter))
        return static_cast<void*>(const_cast< Interactive_Parameter*>(this));
    return QDialog::qt_metacast(_clname);
}

int Interactive_Parameter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

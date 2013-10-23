/****************************************************************************
** Meta object code from reading C++ file 'componentmaintenance.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/gui/componentmaintenance.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'componentmaintenance.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Componentmaintenance[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      22,   21,   21,   21, 0x05,
      45,   21,   21,   21, 0x05,
      65,   21,   21,   21, 0x05,
      81,   21,   21,   21, 0x05,

 // slots: signature, parameters, type, tag, flags
     109,   21,   21,   21, 0x0a,
     134,   21,   21,   21, 0x0a,
     156,   21,   21,   21, 0x0a,
     170,   21,   21,   21, 0x0a,
     182,   21,   21,   21, 0x0a,
     196,   21,   21,   21, 0x0a,
     211,   21,   21,   21, 0x0a,
     223,  218,   21,   21, 0x0a,
     268,  218,   21,   21, 0x0a,
     316,  307,   21,   21, 0x0a,
     355,  307,   21,   21, 0x0a,
     399,   21,   21,   21, 0x0a,
     439,  307,   21,   21, 0x0a,
     481,  473,   21,   21, 0x0a,
     515,   21,   21,   21, 0x0a,
     538,   21,   21,   21, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Componentmaintenance[] = {
    "Componentmaintenance\0\0remove_components(int)\0"
    "add_components(int)\0signal_update()\0"
    "componentmaintenance_died()\0"
    "slot_remove_components()\0slot_add_components()\0"
    "slot_update()\0slot_info()\0slot_couple()\0"
    "slot_monitor()\0quit()\0,col\0"
    "slot_param_doubleclick(QTreeWidgetItem*,int)\0"
    "slot_param_click(QTreeWidgetItem*,int)\0"
    "item,col\0slot_param_press(QTreeWidgetItem*,int)\0"
    "slot_param_rightpress(QTreeWidgetItem*,int)\0"
    "slot_param_rightclick(QTreeWidgetItem*)\0"
    "slot_rename(QTreeWidgetItem*,int)\0"
    "x,y,w,h\0slot_setposition(int,int,int,int)\0"
    "slot_update_monitors()\0slot_atomwizard()\0"
};

void Componentmaintenance::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Componentmaintenance *_t = static_cast<Componentmaintenance *>(_o);
        switch (_id) {
        case 0: _t->remove_components((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->add_components((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->signal_update(); break;
        case 3: _t->componentmaintenance_died(); break;
        case 4: _t->slot_remove_components(); break;
        case 5: _t->slot_add_components(); break;
        case 6: _t->slot_update(); break;
        case 7: _t->slot_info(); break;
        case 8: _t->slot_couple(); break;
        case 9: _t->slot_monitor(); break;
        case 10: _t->quit(); break;
        case 11: _t->slot_param_doubleclick((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 12: _t->slot_param_click((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 13: _t->slot_param_press((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 14: _t->slot_param_rightpress((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 15: _t->slot_param_rightclick((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        case 16: _t->slot_rename((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 17: _t->slot_setposition((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 18: _t->slot_update_monitors(); break;
        case 19: _t->slot_atomwizard(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Componentmaintenance::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Componentmaintenance::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Componentmaintenance,
      qt_meta_data_Componentmaintenance, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Componentmaintenance::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Componentmaintenance::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Componentmaintenance::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Componentmaintenance))
        return static_cast<void*>(const_cast< Componentmaintenance*>(this));
    return QWidget::qt_metacast(_clname);
}

int Componentmaintenance::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    }
    return _id;
}

// SIGNAL 0
void Componentmaintenance::remove_components(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Componentmaintenance::add_components(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Componentmaintenance::signal_update()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void Componentmaintenance::componentmaintenance_died()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}
QT_END_MOC_NAMESPACE

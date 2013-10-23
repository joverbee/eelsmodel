/****************************************************************************
** Meta object code from reading C++ file 'eelsmodel.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/core/eelsmodel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'eelsmodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Eelsmodel[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      29,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x05,
      43,   41,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      61,   10,   10,   10, 0x0a,
      72,   10,   10,   10, 0x0a,
     113,   97,   82,   10, 0x0a,
     147,  138,   82,   10, 0x2a,
     167,   10,   82,   10, 0x2a,
     176,   10,   10,   10, 0x0a,
     190,   10,   10,   10, 0x0a,
     213,   10,   10,   10, 0x0a,
     257,   10,   10,   10, 0x0a,
     298,   10,   10,   10, 0x0a,
     334,   10,   10,   10, 0x0a,
     366,   10,   10,   10, 0x0a,
     377,   10,   10,   10, 0x0a,
     392,   10,   10,   10, 0x0a,
     414,   10,   10,   10, 0x0a,
     430,   10,   10,   10, 0x0a,
     440,   10,   10,   10, 0x0a,
     455,   10,   10,   10, 0x0a,
     475,   10,   10,   10, 0x0a,
     493,   10,   10,   10, 0x0a,
     508,   10,   10,   10, 0x0a,
     528,  138,   10,   10, 0x0a,
     573,   10,   10,   10, 0x0a,
     593,   10,   10,   10, 0x0a,
     615,   10,   10,   10, 0x0a,
     648,   10,   10,   10, 0x0a,
     673,   10,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Eelsmodel[] = {
    "Eelsmodel\0\0componentmaintenance_update()\0"
    "b\0enablemodel(bool)\0newgraph()\0openmsa()\0"
    "Multispectrum*\0filename,silent\0"
    "openDM(std::string,bool)\0filename\0"
    "openDM(std::string)\0openDM()\0newspectrum()\0"
    "componentmaintenance()\0"
    "componentmaintenance_remove_components(int)\0"
    "componentmaintenance_add_components(int)\0"
    "componentmaintenance_updatescreen()\0"
    "componentmaintenance_doupdate()\0"
    "newmodel()\0iterativefit()\0"
    "fitter_updatescreen()\0undoselection()\0"
    "exclude()\0resetexclude()\0slot_save_project()\0"
    "slot_save_model()\0slot_save_as()\0"
    "slot_open_project()\0"
    "slot_open_project_from_filename(std::string)\0"
    "slot_model_update()\0slot_model_detector()\0"
    "slot_componentmaintenance_died()\0"
    "slot_fitterdialog_died()\0"
    "slot_componentmaintenance_updatemonitors()\0"
};

void Eelsmodel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Eelsmodel *_t = static_cast<Eelsmodel *>(_o);
        switch (_id) {
        case 0: _t->componentmaintenance_update(); break;
        case 1: _t->enablemodel((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->newgraph(); break;
        case 3: _t->openmsa(); break;
        case 4: { Multispectrum* _r = _t->openDM((*reinterpret_cast< std::string(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< Multispectrum**>(_a[0]) = _r; }  break;
        case 5: { Multispectrum* _r = _t->openDM((*reinterpret_cast< std::string(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< Multispectrum**>(_a[0]) = _r; }  break;
        case 6: { Multispectrum* _r = _t->openDM();
            if (_a[0]) *reinterpret_cast< Multispectrum**>(_a[0]) = _r; }  break;
        case 7: _t->newspectrum(); break;
        case 8: _t->componentmaintenance(); break;
        case 9: _t->componentmaintenance_remove_components((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->componentmaintenance_add_components((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->componentmaintenance_updatescreen(); break;
        case 12: _t->componentmaintenance_doupdate(); break;
        case 13: _t->newmodel(); break;
        case 14: _t->iterativefit(); break;
        case 15: _t->fitter_updatescreen(); break;
        case 16: _t->undoselection(); break;
        case 17: _t->exclude(); break;
        case 18: _t->resetexclude(); break;
        case 19: _t->slot_save_project(); break;
        case 20: _t->slot_save_model(); break;
        case 21: _t->slot_save_as(); break;
        case 22: _t->slot_open_project(); break;
        case 23: _t->slot_open_project_from_filename((*reinterpret_cast< std::string(*)>(_a[1]))); break;
        case 24: _t->slot_model_update(); break;
        case 25: _t->slot_model_detector(); break;
        case 26: _t->slot_componentmaintenance_died(); break;
        case 27: _t->slot_fitterdialog_died(); break;
        case 28: _t->slot_componentmaintenance_updatemonitors(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Eelsmodel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Eelsmodel::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Eelsmodel,
      qt_meta_data_Eelsmodel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Eelsmodel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Eelsmodel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Eelsmodel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Eelsmodel))
        return static_cast<void*>(const_cast< Eelsmodel*>(this));
    return QWidget::qt_metacast(_clname);
}

int Eelsmodel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    }
    return _id;
}

// SIGNAL 0
void Eelsmodel::componentmaintenance_update()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void Eelsmodel::enablemodel(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE

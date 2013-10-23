/****************************************************************************
** Meta object code from reading C++ file 'fitter_dialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/gui/fitter_dialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'fitter_dialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Fitter_dialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      29,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   15,   14,   14, 0x05,
      41,   14,   14,   14, 0x05,
      50,   14,   14,   14, 0x05,
      78,   14,   14,   14, 0x05,
     104,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
     131,   14,   14,   14, 0x0a,
     160,   14,   14,   14, 0x0a,
     189,   14,   14,   14, 0x0a,
     219,   14,   14,   14, 0x0a,
     250,   14,   14,   14, 0x0a,
     276,   14,   14,   14, 0x0a,
     283,   14,   14,   14, 0x0a,
     301,   14,   14,   14, 0x0a,
     313,   14,   14,   14, 0x0a,
     327,   14,   14,   14, 0x0a,
     357,   14,   14,   14, 0x0a,
     385,   14,   14,   14, 0x0a,
     409,   14,   14,   14, 0x0a,
     430,   14,   14,   14, 0x0a,
     454,   14,   14,   14, 0x0a,
     480,  478,   14,   14, 0x0a,
     512,   14,   14,   14, 0x0a,
     537,   14,   14,   14, 0x0a,
     553,  549,   14,   14, 0x0a,
     579,   14,   14,   14, 0x0a,
     593,   14,   14,   14, 0x0a,
     616,   14,   14,   14, 0x0a,
     638,   14,   14,   14, 0x0a,
     661,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Fitter_dialog[] = {
    "Fitter_dialog\0\0n\0signal_progressbar(int)\0"
    "update()\0signal_start_multi_report()\0"
    "signal_add_multi_report()\0"
    "signal_fitterdialog_died()\0"
    "slot_change_minstep(QString)\0"
    "slot_change_maxstep(QString)\0"
    "slot_change_fraction(QString)\0"
    "slot_change_tolerance(QString)\0"
    "slot_change_nmax(QString)\0quit()\0"
    "slot_iterations()\0slot_stop()\0"
    "slot_update()\0slot_set_update_allways(bool)\0"
    "slot_set_usegradients(bool)\0"
    "slot_set_residual(bool)\0slot_set_multi(bool)\0"
    "slot_set_lintrick(bool)\0slot_set_previous(bool)\0"
    "b\0slot_set_startfromcurrent(bool)\0"
    "slot_set_redrawfit(bool)\0slot_undo()\0"
    "x,y\0slot_setposition(int,int)\0"
    "slot_params()\0slot_params_save_dat()\0"
    "slot_model_save_dat()\0slot_params_save_txt()\0"
    "slot_params_save_crlb_dat()\0"
};

void Fitter_dialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Fitter_dialog *_t = static_cast<Fitter_dialog *>(_o);
        switch (_id) {
        case 0: _t->signal_progressbar((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->update(); break;
        case 2: _t->signal_start_multi_report(); break;
        case 3: _t->signal_add_multi_report(); break;
        case 4: _t->signal_fitterdialog_died(); break;
        case 5: _t->slot_change_minstep((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->slot_change_maxstep((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->slot_change_fraction((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->slot_change_tolerance((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->slot_change_nmax((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->quit(); break;
        case 11: _t->slot_iterations(); break;
        case 12: _t->slot_stop(); break;
        case 13: _t->slot_update(); break;
        case 14: _t->slot_set_update_allways((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 15: _t->slot_set_usegradients((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 16: _t->slot_set_residual((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 17: _t->slot_set_multi((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 18: _t->slot_set_lintrick((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 19: _t->slot_set_previous((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 20: _t->slot_set_startfromcurrent((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 21: _t->slot_set_redrawfit((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 22: _t->slot_undo(); break;
        case 23: _t->slot_setposition((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 24: _t->slot_params(); break;
        case 25: _t->slot_params_save_dat(); break;
        case 26: _t->slot_model_save_dat(); break;
        case 27: _t->slot_params_save_txt(); break;
        case 28: _t->slot_params_save_crlb_dat(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Fitter_dialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Fitter_dialog::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Fitter_dialog,
      qt_meta_data_Fitter_dialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Fitter_dialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Fitter_dialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Fitter_dialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Fitter_dialog))
        return static_cast<void*>(const_cast< Fitter_dialog*>(this));
    return QWidget::qt_metacast(_clname);
}

int Fitter_dialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void Fitter_dialog::signal_progressbar(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Fitter_dialog::update()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void Fitter_dialog::signal_start_multi_report()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void Fitter_dialog::signal_add_multi_report()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void Fitter_dialog::signal_fitterdialog_died()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}
QT_END_MOC_NAMESPACE

/****************************************************************************
** Meta object code from reading C++ file 'menu.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/gui/menu.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'menu.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MenuEelsmodel[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      51,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      24,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,
      32,   14,   14,   14, 0x05,
      43,   14,   14,   14, 0x05,
      59,   14,   14,   14, 0x05,
      74,   14,   14,   14, 0x05,
      94,   14,   14,   14, 0x05,
     107,   14,   14,   14, 0x05,
     127,   14,   14,   14, 0x05,
     145,   14,   14,   14, 0x05,
     164,   14,   14,   14, 0x05,
     179,   14,   14,   14, 0x05,
     200,   14,   14,   14, 0x05,
     215,   14,   14,   14, 0x05,
     235,   14,   14,   14, 0x05,
     252,   14,   14,   14, 0x05,
     281,   14,   14,   14, 0x05,
     299,   14,   14,   14, 0x05,
     320,   14,   14,   14, 0x05,
     342,   14,   14,   14, 0x05,
     359,   14,   14,   14, 0x05,
     376,   14,   14,   14, 0x05,
     396,   14,   14,   14, 0x05,
     411,   14,   14,   14, 0x05,
     426,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
     441,   14,   14,   14, 0x0a,
     455,   14,   14,   14, 0x0a,
     473,   14,   14,   14, 0x0a,
     490,   14,   14,   14, 0x0a,
     512,   14,   14,   14, 0x0a,
     527,   14,   14,   14, 0x0a,
     549,   14,   14,   14, 0x0a,
     570,   14,   14,   14, 0x0a,
     587,   14,   14,   14, 0x0a,
     611,   14,   14,   14, 0x0a,
     627,   14,   14,   14, 0x0a,
     643,   14,   14,   14, 0x0a,
     658,   14,   14,   14, 0x0a,
     672,   14,   14,   14, 0x0a,
     687,   14,   14,   14, 0x0a,
     703,   14,   14,   14, 0x0a,
     727,   14,   14,   14, 0x0a,
     745,   14,   14,   14, 0x0a,
     775,  768,   14,   14, 0x0a,
     797,  768,   14,   14, 0x0a,
     821,   14,   14,   14, 0x0a,
     837,   14,   14,   14, 0x0a,
     855,   14,   14,   14, 0x0a,
     870,   14,   14,   14, 0x0a,
     891,   14,   14,   14, 0x0a,
     906,   14,   14,   14, 0x0a,
     928,  926,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MenuEelsmodel[] = {
    "MenuEelsmodel\0\0explain(QString)\0"
    "file_new()\0file_open_msa()\0file_open_DM()\0"
    "file_project_open()\0file_close()\0"
    "file_project_save()\0file_model_save()\0"
    "file_report_save()\0file_save_as()\0"
    "edit_undoselection()\0edit_exclude()\0"
    "edit_resetexclude()\0model_newmodel()\0"
    "model_componentmaintenance()\0"
    "model_fitbyhand()\0model_iterativefit()\0"
    "model_fitteroptions()\0model_detector()\0"
    "toolbar_normal()\0toolbar_selection()\0"
    "toolbar_zoom()\0toolbar_home()\0"
    "toolbar_link()\0slotFileNew()\0"
    "slotFileOpenMSA()\0slotFileOpenDM()\0"
    "slotFileProjectOpen()\0slotFileSave()\0"
    "slotFileProjectSave()\0slotFileReportSave()\0"
    "slotFileSaveAs()\0slotFileProjectSaveAs()\0"
    "slotFileClose()\0slotFilePrint()\0"
    "slotFileQuit()\0slotEditCut()\0"
    "slotEditCopy()\0slotEditPaste()\0"
    "slotEditUndoSelection()\0slotEditExclude()\0"
    "slotEditResetExclude()\0toggle\0"
    "slotViewToolBar(bool)\0slotViewStatusBar(bool)\0"
    "slotHelpAbout()\0slotHelpAboutQt()\0"
    "slotModelNew()\0slotModelComponent()\0"
    "slotModelFit()\0slotModelDETECTOR()\0b\0"
    "slot_enable_model(bool)\0"
};

void MenuEelsmodel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MenuEelsmodel *_t = static_cast<MenuEelsmodel *>(_o);
        switch (_id) {
        case 0: _t->explain((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->file_new(); break;
        case 2: _t->file_open_msa(); break;
        case 3: _t->file_open_DM(); break;
        case 4: _t->file_project_open(); break;
        case 5: _t->file_close(); break;
        case 6: _t->file_project_save(); break;
        case 7: _t->file_model_save(); break;
        case 8: _t->file_report_save(); break;
        case 9: _t->file_save_as(); break;
        case 10: _t->edit_undoselection(); break;
        case 11: _t->edit_exclude(); break;
        case 12: _t->edit_resetexclude(); break;
        case 13: _t->model_newmodel(); break;
        case 14: _t->model_componentmaintenance(); break;
        case 15: _t->model_fitbyhand(); break;
        case 16: _t->model_iterativefit(); break;
        case 17: _t->model_fitteroptions(); break;
        case 18: _t->model_detector(); break;
        case 19: _t->toolbar_normal(); break;
        case 20: _t->toolbar_selection(); break;
        case 21: _t->toolbar_zoom(); break;
        case 22: _t->toolbar_home(); break;
        case 23: _t->toolbar_link(); break;
        case 24: _t->slotFileNew(); break;
        case 25: _t->slotFileOpenMSA(); break;
        case 26: _t->slotFileOpenDM(); break;
        case 27: _t->slotFileProjectOpen(); break;
        case 28: _t->slotFileSave(); break;
        case 29: _t->slotFileProjectSave(); break;
        case 30: _t->slotFileReportSave(); break;
        case 31: _t->slotFileSaveAs(); break;
        case 32: _t->slotFileProjectSaveAs(); break;
        case 33: _t->slotFileClose(); break;
        case 34: _t->slotFilePrint(); break;
        case 35: _t->slotFileQuit(); break;
        case 36: _t->slotEditCut(); break;
        case 37: _t->slotEditCopy(); break;
        case 38: _t->slotEditPaste(); break;
        case 39: _t->slotEditUndoSelection(); break;
        case 40: _t->slotEditExclude(); break;
        case 41: _t->slotEditResetExclude(); break;
        case 42: _t->slotViewToolBar((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 43: _t->slotViewStatusBar((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 44: _t->slotHelpAbout(); break;
        case 45: _t->slotHelpAboutQt(); break;
        case 46: _t->slotModelNew(); break;
        case 47: _t->slotModelComponent(); break;
        case 48: _t->slotModelFit(); break;
        case 49: _t->slotModelDETECTOR(); break;
        case 50: _t->slot_enable_model((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MenuEelsmodel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MenuEelsmodel::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MenuEelsmodel,
      qt_meta_data_MenuEelsmodel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MenuEelsmodel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MenuEelsmodel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MenuEelsmodel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MenuEelsmodel))
        return static_cast<void*>(const_cast< MenuEelsmodel*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MenuEelsmodel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 51)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 51;
    }
    return _id;
}

// SIGNAL 0
void MenuEelsmodel::explain(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MenuEelsmodel::file_new()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void MenuEelsmodel::file_open_msa()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void MenuEelsmodel::file_open_DM()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void MenuEelsmodel::file_project_open()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void MenuEelsmodel::file_close()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void MenuEelsmodel::file_project_save()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}

// SIGNAL 7
void MenuEelsmodel::file_model_save()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}

// SIGNAL 8
void MenuEelsmodel::file_report_save()
{
    QMetaObject::activate(this, &staticMetaObject, 8, 0);
}

// SIGNAL 9
void MenuEelsmodel::file_save_as()
{
    QMetaObject::activate(this, &staticMetaObject, 9, 0);
}

// SIGNAL 10
void MenuEelsmodel::edit_undoselection()
{
    QMetaObject::activate(this, &staticMetaObject, 10, 0);
}

// SIGNAL 11
void MenuEelsmodel::edit_exclude()
{
    QMetaObject::activate(this, &staticMetaObject, 11, 0);
}

// SIGNAL 12
void MenuEelsmodel::edit_resetexclude()
{
    QMetaObject::activate(this, &staticMetaObject, 12, 0);
}

// SIGNAL 13
void MenuEelsmodel::model_newmodel()
{
    QMetaObject::activate(this, &staticMetaObject, 13, 0);
}

// SIGNAL 14
void MenuEelsmodel::model_componentmaintenance()
{
    QMetaObject::activate(this, &staticMetaObject, 14, 0);
}

// SIGNAL 15
void MenuEelsmodel::model_fitbyhand()
{
    QMetaObject::activate(this, &staticMetaObject, 15, 0);
}

// SIGNAL 16
void MenuEelsmodel::model_iterativefit()
{
    QMetaObject::activate(this, &staticMetaObject, 16, 0);
}

// SIGNAL 17
void MenuEelsmodel::model_fitteroptions()
{
    QMetaObject::activate(this, &staticMetaObject, 17, 0);
}

// SIGNAL 18
void MenuEelsmodel::model_detector()
{
    QMetaObject::activate(this, &staticMetaObject, 18, 0);
}

// SIGNAL 19
void MenuEelsmodel::toolbar_normal()
{
    QMetaObject::activate(this, &staticMetaObject, 19, 0);
}

// SIGNAL 20
void MenuEelsmodel::toolbar_selection()
{
    QMetaObject::activate(this, &staticMetaObject, 20, 0);
}

// SIGNAL 21
void MenuEelsmodel::toolbar_zoom()
{
    QMetaObject::activate(this, &staticMetaObject, 21, 0);
}

// SIGNAL 22
void MenuEelsmodel::toolbar_home()
{
    QMetaObject::activate(this, &staticMetaObject, 22, 0);
}

// SIGNAL 23
void MenuEelsmodel::toolbar_link()
{
    QMetaObject::activate(this, &staticMetaObject, 23, 0);
}
QT_END_MOC_NAMESPACE

/****************************************************************************
** Meta object code from reading C++ file 'AVClock.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.0.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../AVClock.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AVClock.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.0.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_AVClock_t {
    QByteArrayData data[10];
    char stringdata[62];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_AVClock_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_AVClock_t qt_meta_stringdata_AVClock = {
    {
QT_MOC_LITERAL(0, 0, 7),
QT_MOC_LITERAL(1, 8, 6),
QT_MOC_LITERAL(2, 15, 0),
QT_MOC_LITERAL(3, 16, 7),
QT_MOC_LITERAL(4, 24, 7),
QT_MOC_LITERAL(5, 32, 8),
QT_MOC_LITERAL(6, 41, 5),
QT_MOC_LITERAL(7, 47, 5),
QT_MOC_LITERAL(8, 53, 1),
QT_MOC_LITERAL(9, 55, 5)
    },
    "AVClock\0paused\0\0resumed\0started\0"
    "resetted\0start\0pause\0p\0reset\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AVClock[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x05,
       1,    0,   57,    2, 0x05,
       3,    0,   58,    2, 0x05,
       4,    0,   59,    2, 0x05,
       5,    0,   60,    2, 0x05,

 // slots: name, argc, parameters, tag, flags
       6,    0,   61,    2, 0x0a,
       7,    1,   62,    2, 0x0a,
       9,    0,   65,    2, 0x0a,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void,

       0        // eod
};

void AVClock::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AVClock *_t = static_cast<AVClock *>(_o);
        switch (_id) {
        case 0: _t->paused((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->paused(); break;
        case 2: _t->resumed(); break;
        case 3: _t->started(); break;
        case 4: _t->resetted(); break;
        case 5: _t->start(); break;
        case 6: _t->pause((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->reset(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (AVClock::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AVClock::paused)) {
                *result = 0;
            }
        }
        {
            typedef void (AVClock::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AVClock::paused)) {
                *result = 1;
            }
        }
        {
            typedef void (AVClock::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AVClock::resumed)) {
                *result = 2;
            }
        }
        {
            typedef void (AVClock::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AVClock::started)) {
                *result = 3;
            }
        }
        {
            typedef void (AVClock::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AVClock::resetted)) {
                *result = 4;
            }
        }
    }
}

const QMetaObject AVClock::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_AVClock.data,
      qt_meta_data_AVClock,  qt_static_metacall, 0, 0}
};


const QMetaObject *AVClock::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AVClock::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AVClock.stringdata))
        return static_cast<void*>(const_cast< AVClock*>(this));
    return QObject::qt_metacast(_clname);
}

int AVClock::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void AVClock::paused(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void AVClock::paused()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void AVClock::resumed()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void AVClock::started()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void AVClock::resetted()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}
QT_END_MOC_NAMESPACE

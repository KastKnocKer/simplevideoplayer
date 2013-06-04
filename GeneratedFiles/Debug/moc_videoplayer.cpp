/****************************************************************************
** Meta object code from reading C++ file 'videoplayer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.0.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../videoplayer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'videoplayer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.0.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_videoplayer_t {
    QByteArrayData data[18];
    char stringdata[144];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_videoplayer_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_videoplayer_t qt_meta_stringdata_videoplayer = {
    {
QT_MOC_LITERAL(0, 0, 11),
QT_MOC_LITERAL(1, 12, 10),
QT_MOC_LITERAL(2, 23, 0),
QT_MOC_LITERAL(3, 24, 4),
QT_MOC_LITERAL(4, 29, 5),
QT_MOC_LITERAL(5, 35, 8),
QT_MOC_LITERAL(6, 44, 4),
QT_MOC_LITERAL(7, 49, 5),
QT_MOC_LITERAL(8, 55, 6),
QT_MOC_LITERAL(9, 62, 7),
QT_MOC_LITERAL(10, 70, 4),
QT_MOC_LITERAL(11, 75, 4),
QT_MOC_LITERAL(12, 80, 4),
QT_MOC_LITERAL(13, 85, 11),
QT_MOC_LITERAL(14, 97, 11),
QT_MOC_LITERAL(15, 109, 9),
QT_MOC_LITERAL(16, 119, 12),
QT_MOC_LITERAL(17, 132, 10)
    },
    "videoplayer\0first_play\0\0open\0about\0"
    "loadFile\0quit\0pause\0resume\0playing\0"
    "seek\0incr\0tick\0resetSlider\0slider_seek\0"
    "stop_tick\0histoClicked\0openDialog\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_videoplayer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   89,    2, 0x05,

 // slots: name, argc, parameters, tag, flags
       3,    0,   90,    2, 0x0a,
       4,    0,   91,    2, 0x0a,
       5,    0,   92,    2, 0x0a,
       6,    0,   93,    2, 0x0a,
       7,    0,   94,    2, 0x0a,
       8,    0,   95,    2, 0x0a,
       9,    0,   96,    2, 0x0a,
      10,    1,   97,    2, 0x0a,
      12,    0,  100,    2, 0x0a,
      13,    0,  101,    2, 0x0a,
      14,    0,  102,    2, 0x0a,
      15,    0,  103,    2, 0x0a,
      16,    0,  104,    2, 0x0a,
      17,    0,  105,    2, 0x0a,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void videoplayer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        videoplayer *_t = static_cast<videoplayer *>(_o);
        switch (_id) {
        case 0: _t->first_play(); break;
        case 1: _t->open(); break;
        case 2: _t->about(); break;
        case 3: _t->loadFile(); break;
        case 4: _t->quit(); break;
        case 5: _t->pause(); break;
        case 6: _t->resume(); break;
        case 7: _t->playing(); break;
        case 8: _t->seek((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->tick(); break;
        case 10: _t->resetSlider(); break;
        case 11: _t->slider_seek(); break;
        case 12: _t->stop_tick(); break;
        case 13: _t->histoClicked(); break;
        case 14: _t->openDialog(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (videoplayer::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&videoplayer::first_play)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject videoplayer::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_videoplayer.data,
      qt_meta_data_videoplayer,  qt_static_metacall, 0, 0}
};


const QMetaObject *videoplayer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *videoplayer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_videoplayer.stringdata))
        return static_cast<void*>(const_cast< videoplayer*>(this));
    return QWidget::qt_metacast(_clname);
}

int videoplayer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void videoplayer::first_play()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE

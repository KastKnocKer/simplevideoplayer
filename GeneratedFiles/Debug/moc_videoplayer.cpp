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
    QByteArrayData data[7];
    char stringdata[44];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_videoplayer_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_videoplayer_t qt_meta_stringdata_videoplayer = {
    {
QT_MOC_LITERAL(0, 0, 11),
QT_MOC_LITERAL(1, 12, 4),
QT_MOC_LITERAL(2, 17, 0),
QT_MOC_LITERAL(3, 18, 5),
QT_MOC_LITERAL(4, 24, 4),
QT_MOC_LITERAL(5, 29, 4),
QT_MOC_LITERAL(6, 34, 8)
    },
    "videoplayer\0open\0\0about\0tick\0time\0"
    "loadFile\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_videoplayer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x0a,
       3,    0,   35,    2, 0x0a,
       4,    1,   36,    2, 0x0a,
       6,    0,   39,    2, 0x0a,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::LongLong,    5,
    QMetaType::Void,

       0        // eod
};

void videoplayer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        videoplayer *_t = static_cast<videoplayer *>(_o);
        switch (_id) {
        case 0: _t->open(); break;
        case 1: _t->about(); break;
        case 2: _t->tick((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 3: _t->loadFile(); break;
        default: ;
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
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

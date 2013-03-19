#include "videoplayer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	videoplayer w;
	w.show();
	return a.exec();
}

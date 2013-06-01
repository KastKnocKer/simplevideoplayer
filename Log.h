#pragma once

#include <QDialog>
#include <QDialogButtonBox>
#include <QTabWidget>
#include <QVBoxLayout>

#include <stdint.h>


class QTabWidget;
class QDialogButtonBox;
class QVBoxLayout;


class GeneralTab: public QWidget
{
	Q_OBJECT

private:

	//tempo
	int64_t duration;
	int hours;
	int min;
	int sec;
	int us;

	double bitrate;

public:
    explicit GeneralTab(QWidget *parent = 0);
};

class VideoTab: public QWidget
{
	Q_OBJECT

private:

	double fps;

public:
    explicit VideoTab(QWidget *parent = 0);
};

class AudioTab: public QWidget
{
	Q_OBJECT

public:
    explicit AudioTab(QWidget *parent = 0);
};


/**
classe per la visualizzazione di importanti informazioni
riguardanti il video in riproduzione
*/
class Log : public QDialog
{
	Q_OBJECT

private:

	QTabWidget *tabWidget;
	QDialogButtonBox *buttonBox;
	QVBoxLayout *mainLayout;

public:

	explicit Log(QWidget *parent = 0);
	~Log(void);

};


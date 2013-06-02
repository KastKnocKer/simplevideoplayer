#pragma once

#include <QDialog>
#include <QDialogButtonBox>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTime>

#include <stdint.h>

#include "VideoState.h"


class QTabWidget;
class QDialogButtonBox;
class QVBoxLayout;
class QLabel;
class QLineEdit;
class QTime;


class GeneralTab: public QWidget
{
	Q_OBJECT

private:

	//tempo
	int64_t duration;
	int hours;
	int mins;
	int secs;
	int us;

	double bitrate;

public:
    explicit GeneralTab(VideoState *is, QWidget *parent = 0);
};

class VideoTab: public QWidget
{
	Q_OBJECT

private:

	double fps;

public:
    explicit VideoTab(VideoState *is, QWidget *parent = 0);
};

class AudioTab: public QWidget
{
	Q_OBJECT

private:

public:
    explicit AudioTab(VideoState *is, QWidget *parent = 0);
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

	explicit Log(VideoState *is, QWidget *parent = 0);
	~Log(void);

	/**
		Imposta il riferimento all'oggetto che mantiene lo stato del video
		@param is riferimento all'oggetto che mantiene lo stato del video
	*/
	void SetVideoState(VideoState *is);

};


#include "Log.h"


Log::Log(VideoState *is, QWidget *parent):QDialog(parent)
{

	//generazione del tab e aggiunta delle varie pagine
	tabWidget = new QTabWidget;
	tabWidget->addTab(new GeneralTab(is), tr("General Info"));
	tabWidget->addTab(new VideoTab(is), tr("Video"));
	tabWidget->addTab(new AudioTab(is), tr("Audio"));

	//bottone ok, se premuto chiude dialog
	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

	mainLayout = new QVBoxLayout();
    mainLayout->setSizeConstraint(QLayout::SetNoConstraint);
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setWindowTitle(tr("Info"));
	setMinimumSize(400, 400);
}


Log::~Log(void)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////
// GENERAL TAB
/////////////////////////////////////////////////////////////////////////////////////////////

GeneralTab::GeneralTab(VideoState *is, QWidget *parent):QWidget(parent)
{

	QLabel *fileNameLabel = new QLabel(tr("File Name:"));
	QLineEdit *fileNameEdit = new QLineEdit(QString::fromStdString(is->getSourceFilename()));

	///////////////////////////////////////////////////////////
	
	if (is->pFormatCtx->duration != AV_NOPTS_VALUE){
		//caso in cui riesco a leggere valore dall'header
		duration = is->pFormatCtx->duration + 5000;
	} else {
		//caso peggiore, devo calcolarmi io la durata
		duration = (int) av_rescale(is->totalFramesNumber, is->video_st->time_base.num, 
			is->video_st->time_base.den);
	}

	secs = duration / AV_TIME_BASE;
    us = duration % AV_TIME_BASE;
	mins = secs / 60;
    secs %= 60;
    hours = mins / 60;
    mins %= 60;
	QLabel *durationLabel = new QLabel(tr("Duration:"));
	QTime displayTime(hours, mins, secs);
	QLineEdit *durationEdit = new QLineEdit(displayTime.toString());

	///////////////////////////////////////////////////////

	QLabel *starttimeLabel = new QLabel(tr("Start Time: [s]"));
	QLineEdit *starttimeEdit;
	if(is->pFormatCtx->start_time != AV_NOPTS_VALUE){

		int secs = is->pFormatCtx->start_time / AV_TIME_BASE;
		int us = abs(is->pFormatCtx->start_time % AV_TIME_BASE);
		int us_ok = (int) av_rescale(us, 1000000, AV_TIME_BASE);
		starttimeEdit = new QLineEdit(QString::number(secs) + "." + QString::number(us_ok));
	}
	else{
		starttimeEdit = new QLineEdit("N/A");
	}

	QLabel *bitrateLabel = new QLabel(tr("BitRate:"));
	QLineEdit *bitrateEdit;
	if(is->pFormatCtx->bit_rate){
		bitrateEdit = new QLineEdit(QString::number(is->pFormatCtx->bit_rate) + " kb/s");
	} else {
		bitrateEdit = new QLineEdit("N/A");
	}
	///////////////////////////////////////////////////////

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(fileNameLabel);
    mainLayout->addWidget(fileNameEdit);
	mainLayout->addWidget(durationLabel);
	mainLayout->addWidget(durationEdit);
	mainLayout->addWidget(starttimeLabel);
	mainLayout->addWidget(starttimeEdit);
	mainLayout->addWidget(bitrateLabel);
	mainLayout->addWidget(bitrateEdit);

	mainLayout->addStretch(1);

	setLayout(mainLayout);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// VIDEO TAB
/////////////////////////////////////////////////////////////////////////////////////////////

VideoTab::VideoTab(VideoState *is, QWidget *parent):QWidget(parent)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////
// AUDIO TAB
/////////////////////////////////////////////////////////////////////////////////////////////

AudioTab::AudioTab(VideoState *is, QWidget *parent):QWidget(parent)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////
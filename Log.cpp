#include "Log.h"


Log::Log(QWidget *parent):QDialog(parent)
{

	//generazione del tab e aggiunta delle varie pagine
	tabWidget = new QTabWidget;
	tabWidget->addTab(new GeneralTab(), tr("General Info"));
	tabWidget->addTab(new VideoTab(), tr("Video"));
	tabWidget->addTab(new AudioTab(), tr("Audio"));

	//bottone ok, se premuto chiude dialog
	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

	mainLayout = new QVBoxLayout();
    mainLayout->setSizeConstraint(QLayout::SetNoConstraint);
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setWindowTitle(tr("Info"));
}


Log::~Log(void)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////
// GENERAL TAB
/////////////////////////////////////////////////////////////////////////////////////////////

GeneralTab::GeneralTab(QWidget *parent):QWidget(parent)
{


}

/////////////////////////////////////////////////////////////////////////////////////////////
// VIDEO TAB
/////////////////////////////////////////////////////////////////////////////////////////////

VideoTab::VideoTab(QWidget *parent):QWidget(parent)
{


}

/////////////////////////////////////////////////////////////////////////////////////////////
// AUDIO TAB
/////////////////////////////////////////////////////////////////////////////////////////////

AudioTab::AudioTab(QWidget *parent):QWidget(parent)
{


}
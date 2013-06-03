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

	///////////////////////////////////////////////////////

	QLabel *bitrateLabel = new QLabel(tr("BitRate:"));
	QLineEdit *bitrateEdit;
	if(is->pFormatCtx->bit_rate){
		bitrateEdit = new QLineEdit(QString::number(is->pFormatCtx->bit_rate) + " kb/s");
	} else {
		bitrateEdit = new QLineEdit("N/A");
	}

	///////////////////////////////////////////////////////

	std::vector<QLabel *> labelchapter;
	std::vector<QLineEdit *> editchapter;
	for (int i=0; i<is->pFormatCtx->nb_chapters; ++i) {
        AVChapter *ch = is->pFormatCtx->chapters[i];
        QString start("start " + QString::number(ch->start * av_q2d(ch->time_base)) + ", ");
        QString end("end " + QString::number(ch->end   * av_q2d(ch->time_base)));

		labelchapter[i] = new QLabel("Chapter " + QString::number(0) + "." + QString::number(i) + ":");
		editchapter[i] = new QLineEdit(start + end);

        //dump_metadata(NULL, ch->metadata, "    ");
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
	for (int i=0; i<is->pFormatCtx->nb_chapters; ++i) {
		mainLayout->addWidget(labelchapter[i]);
		mainLayout->addWidget(editchapter[i]);
	}

	mainLayout->addStretch(1);

	setLayout(mainLayout);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// VIDEO TAB
/////////////////////////////////////////////////////////////////////////////////////////////

VideoTab::VideoTab(VideoState *is, QWidget *parent):QWidget(parent)
{

	AVStream *video_st = is->video_st;

	int g = av_gcd(video_st->time_base.num, video_st->time_base.den);
	QLabel *numFrameDemuxedlabel = new QLabel(tr("Number of Frames Demuxed for Header:"));
	QLineEdit *numFrameDemuxededit = new QLineEdit(QString::number(video_st->codec_info_nb_frames) + ",  "
		+ video_st->time_base.num/g + "/" + video_st->time_base.den/g);


	///////////////////////////////////////////////////////

	QLabel *codec_type_label = new QLabel(tr("Codec type:"));
	QLineEdit *codec_type_edit = new QLineEdit(av_get_media_type_string(video_st->codec->codec_type));

	///////////////////////////////////////////////////////

	QLabel *codec_name_label = new QLabel(tr("Codec name:"));
	QLineEdit *codec_name_edit = new QLineEdit(avcodec_get_name(video_st->codec->codec_id));

	///////////////////////////////////////////////////////

	const AVCodec *p;
	const char *profile = NULL;
	QLabel *profile_label;
	QLineEdit *profile_edit;
	if (video_st->codec->profile != FF_PROFILE_UNKNOWN) {
		if (video_st->codec->codec)
			p = video_st->codec->codec;
		else
			p = avcodec_find_decoder(video_st->codec->codec_id);
		if (p){
			profile = av_get_profile_name(p, video_st->codec->profile);
			profile_label = new QLabel(tr("Profile:"));
			profile_edit = new QLineEdit(tr(profile));
		}
    }

	///////////////////////////////////////////////////////

	QLineEdit *codec_tag_edit;
	QLabel *codec_tag_label;
	if (is->video_st->codec->codec_tag){
		char tag_buf[32];
		av_get_codec_tag_string(tag_buf, sizeof(tag_buf), video_st->codec->codec_tag);
		codec_tag_label = new QLabel(tr("Codec TAG:"));
		codec_tag_edit = new QLineEdit(tr(tag_buf) + " / " + video_st->codec->codec_tag);
    }

	///////////////////////////////////////////////////////

	QLabel *pixelformat_label = NULL;
	QLineEdit *pixelformat_edit;
	QLineEdit *pixelformat_edit2 = NULL;

	if (video_st->codec->pix_fmt != AV_PIX_FMT_NONE) {
		pixelformat_label = new QLabel(tr("Pixel Format:"));
		pixelformat_edit = new QLineEdit(tr(av_get_pix_fmt_name(video_st->codec->pix_fmt)));
		if (video_st->codec->bits_per_raw_sample && 
			video_st->codec->bits_per_raw_sample <= av_pix_fmt_desc_get(video_st->codec->pix_fmt)->comp[0].depth_minus1)
			pixelformat_edit2 = new QLineEdit(tr("(") + QString::number(video_st->codec->bits_per_raw_sample) + tr(" bpc)"));
    }

	///////////////////////////////////////////////////////



	///////////////////////////////////////////////////////

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(numFrameDemuxedlabel);
    mainLayout->addWidget(numFrameDemuxededit);
	mainLayout->addWidget(codec_type_label);
    mainLayout->addWidget(codec_type_edit);
	mainLayout->addWidget(codec_name_label);
    mainLayout->addWidget(codec_name_edit);
	if(profile != NULL){
		mainLayout->addWidget(profile_label);
		mainLayout->addWidget(profile_edit);
	}
	if(codec_tag_edit != NULL){
		mainLayout->addWidget(codec_tag_label);
		mainLayout->addWidget(codec_tag_edit);
	}
	if (pixelformat_label != NULL) {
		mainLayout->addWidget(pixelformat_label);
		mainLayout->addWidget(pixelformat_edit);
		if(pixelformat_edit2 != NULL){
			mainLayout->addWidget(pixelformat_edit2);
		}
	}

	mainLayout->addStretch(1);
	setLayout(mainLayout);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// AUDIO TAB
/////////////////////////////////////////////////////////////////////////////////////////////

AudioTab::AudioTab(VideoState *is, QWidget *parent):QWidget(parent)
{
	AVStream *audio_st = is->audio_st;

	QLabel *numFrameDemuxedlabel = new QLabel(tr("Number of Frames Demuxed for Header:"));
	QLineEdit *numFrameDemuxededit = new QLineEdit(QString::number(audio_st->codec_info_nb_frames));

	///////////////////////////////////////////////////////

	QLabel *codec_type_label = new QLabel(tr("Codec type:"));
	QLineEdit *codec_type_edit = new QLineEdit(av_get_media_type_string(audio_st->codec->codec_type));

	///////////////////////////////////////////////////////

	QLabel *codec_name_label = new QLabel(tr("Codec name:"));
	QLineEdit *codec_name_edit = new QLineEdit(avcodec_get_name(audio_st->codec->codec_id));



	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(numFrameDemuxedlabel);
    mainLayout->addWidget(numFrameDemuxededit);
	mainLayout->addWidget(codec_type_label);
    mainLayout->addWidget(codec_type_edit);
	mainLayout->addWidget(codec_name_label);
    mainLayout->addWidget(codec_name_edit);

	mainLayout->addStretch(1);
	setLayout(mainLayout);


}

/////////////////////////////////////////////////////////////////////////////////////////////
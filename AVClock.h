#ifndef AVCLOCK_H
#define AVLCOCK_H

#include <QTimer>
#include <QElapsedTimer>

static const double kThousandth = 0.001;

/*
 * AVClock is created by AVPlayer. The only way to access AVClock is through AVPlayer::masterClock()
 * The default clock type is Audio's clock, i.e. vedio synchronizes to audio. If audio stream is not
 * detected, then the clock will set to External clock automatically.
 * I name it ExternalClock because the clock can be corrected outside, though it is a clock inside AVClock
 */
class AVClock : public QObject
{
	Q_OBJECT

public:

	typedef enum {
        AudioClock, ExternalClock
    } ClockType;

	void setClockType(ClockType ct);
    ClockType clockType() const;

	bool isActive() const;
    /*
     * auto clock: use audio clock if audio stream found, otherwise use external clock
     */
    void setClockAuto(bool a);
    bool isClockAuto() const;

	/*in seconds*/
    inline double pts() const;
    inline double value() const;				//the real timestamp: pts + delay
    inline void updateValue(double pts);		//update the pts

    /*used when seeking and correcting from external*/
    void updateExternalClock(qint64 msecs);

    /*external clock outside still running, so it's more accurate for syncing multiple clocks serially*/
    void updateExternalClock(const AVClock& clock);

	/* utilizzo inline per velocizzare l'utilizzo di queste funzioni che vengono frequentemente utilizzate */
	inline void updateVideoPts(double pts);
    inline double videoPts() const;
    inline double delay() const; //playing audio spends some time
    inline void updateDelay(double delay);

	AVClock(ClockType c, QObject* parent = 0);
    AVClock(QObject* parent = 0);
	~AVClock(void);

signals:
    void paused(bool);
    void paused(); //equals to paused(true)
    void resumed();//equals to paused(false)
    void started();
    void resetted();

/* A slot is a function that is called in response to a particular signal */
public slots:
    //these slots are not frequently used. so not inline
    /*start the external clock*/
    void start();
    /*pause external clock*/
    void pause(bool p);
    /*reset(stop) external clock*/
    void reset();

private:
    bool auto_clock;
    ClockType clock_type;
    mutable double pts_;	/* If a data member is declared mutable, then it is legal to assign a value to this data member from a const member function */
    double pts_v;
    double delay_;
    mutable QElapsedTimer timer;

};


double AVClock::value() const
{
    if (clock_type == AudioClock) {
        return pts_ + delay_;
    } else {
        if (timer.isValid())
            return pts_ += double(timer.restart()) * kThousandth;
        else {//timer is paused
            qDebug("clock is paused. return the last value %f", pts_);
            return pts_;
        }
    }
}

void AVClock::updateValue(double pts)
{
    if (clock_type == AudioClock)
        pts_ = pts;
}

void AVClock::updateVideoPts(double pts)
{
    pts_v = pts;
}

double AVClock::videoPts() const
{
    return pts_v;
}

double AVClock::delay() const
{
    return delay_;
}

void AVClock::updateDelay(double delay)
{
    delay_ = delay;
}

#endif //AVLCOCK_H

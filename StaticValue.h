
/**
	Dimensione del buffer SDL Audio, in sample. Piu e piccolo e maggiore
	sara la precisione A-V
*/
#define SDL_AUDIO_BUFFER_SIZE 1024

#define VIDEO_PICTURE_QUEUE_SIZE 4 /* numero massimo di elementi nella coda di frame */

#define MAX_AUDIOQ_SIZE (64)	/* numero massimo di elementi nella coda di pacchetti audio */
#define MAX_VIDEOQ_SIZE (64)	/* numero massimo di elementi nelal coda di pacchetti video */

/**
	se ho una differenza A-V minore di questo valore, viene ipotizzato che lo stream
	sia in sincronia
*/
#define AV_SYNC_THRESHOLD 0.01

/**
	non viene effettuata una sincronia se supero questa soglia
*/
#define AV_NOSYNC_THRESHOLD 10.0

/**
	maximum audio speed change to get correct sync
*/
#define SAMPLE_CORRECTION_PERCENT_MAX 10

/**
	We use about AUDIO_DIFF_AVG_NB A-V differences to make the average
*/
#define AUDIO_DIFF_AVG_NB 20




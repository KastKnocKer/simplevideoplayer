#ifndef UTILITY_H
#define UTILITY_H

/**
classe per la gestione di parametri fondamentali per piu processi dell'applicativo
i valori sono privati e accessibili solo tramite apposite chiamate
*/
class Utility{

private:
	int _quit;
	int _pause;

public:

	Utility();
	~Utility();

	void setStopValue(int quit);
	int getStopValue();

};

#endif //UTILITY_H
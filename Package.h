#pragma once
#include "Config.h"

class Package
{
private:
	int id;
	int size;
	float delay;
	float generateTime;
	float terminalTime;
	int destination;


public:
	Package();
	Package(int id, float time);
	~Package();
	void setDestination(int dest) {
		destination = dest;
	}
	int getDestination() {
		return destination;
	}

	float getDelay() {
		return terminalTime - generateTime;
	}

	void setTerminalTime(float time) {
		terminalTime = time;
	}

};


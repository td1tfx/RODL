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
	int hop;

public:
	Package();
	Package(int id, float time);
	~Package();
	void setDestination(int dest) {
		destination = dest;
	}
	int getId() {
		return id;
	}

	int getDestination() {
		return destination;
	}
	int& getHop() {
		return hop;
	}
	int getGenerateTime() {
		return generateTime;
	}

	void setGenerateTime(int time) {
		generateTime = time;
	}

	float getDelay() {
		delay = terminalTime - generateTime;
		return delay;
	}

	void setTerminalTime(float time) {
		terminalTime = time;
	}

};


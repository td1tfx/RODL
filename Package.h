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
	int source;
	int destination;
	int hop;
	double* path;
	bool signaling;

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

	int getSource() {
		return source;
	}

	void setSource(int id) {
		source = id;
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

	int getNextNode() {
		return path[hop];
	}

	void setPathSize(int size) {		
		path = new double[size];
	}

	double& getPathData(int num) {
		return path[num];
	}

	double* getPath() {
		return path;
	}

	void setTerminalTime(float time) {
		terminalTime = time;
	}

	void setSignaling() {
		signaling = true;
		size = Config::getInstance()->getSignalSize();
	}
	bool isSignaling() {
		return signaling;
	}
};


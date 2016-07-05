#include "networkOption.h"
#pragma once
class Config
{
private:
	Config();
	static Config* m_config;
	~Config();
	int packageSize;
	int signalingSize;
	int neuralFirst;
	double maxGenerateRate;	
	//row and column <=10;
	int maxRow;
	int maxColumn;
	int bandwidth;
	int maxPacNumPerNode;
	int round;
	int testRound;
	int fullMod;
	int singleDestMod;
	int singleOutputMod;

	NetworkOption _option;
public:
	
	static Config* getInstance() {
		if (m_config == NULL)  
			m_config = new Config();
		return m_config;
	}
	int getPackageSize() {
		return packageSize;
	}
	int getSignalSize() {
		return signalingSize;
	}

	int getNeuralFirst() {
		return neuralFirst;
	}
	int getMaxRow() {
		return maxRow;
	}
	int getMaxColumn() {
		return maxColumn;
	}

	int getBandwidth() {
		return bandwidth;
	}

	double getMaxGenerateRate() {
		return maxGenerateRate;
	}
	
	void resetMaxGenerateRate(double rate) {
		maxGenerateRate = rate;
	}

	int gerMaxPacNumPerNode() {
		return maxPacNumPerNode;
	}

	int getRound() {
		return round;
	}
	int getTestRound() {
		return testRound;
	}

	int isFullMod() {
		return fullMod;
	}

	int isSingleDestMod() {
		return singleDestMod;
	}

	int isSingleOutputMod() {
		return singleOutputMod;
	}
	//void init();
};




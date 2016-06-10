#pragma once
class Config
{
private:
	Config();
	static Config* m_config;
	~Config();
	int packageSize;
	int neuralFirst;
	int maxGenerateRate;
	//row and column <=10;
	int maxRow;
	int maxColumn;
	int bandwidth;
	int maxPacNumPerNode;
	int round;
	int testRound;

public:
	
	static Config* getInstance() {
		if (m_config == NULL)  
			m_config = new Config();
		return m_config;
	}
	int getPackageSize() {
		return packageSize;
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

	int getMaxGenerateRate() {
		return maxGenerateRate;
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
	//void init();
};




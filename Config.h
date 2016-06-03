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
	//void init();
};




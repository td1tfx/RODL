#pragma once
class Config
{
private:
	Config();
	static Config* m_config;
	~Config();
	int packageSize;
	int neuralFirst;
	//row and column <=10;
	int maxRow;
	int maxColumn;
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
	//void init();
};




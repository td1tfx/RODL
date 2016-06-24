#include "stdafx.h"
#include "Node.h"


Node::Node()
{
	qServe = new std::queue<Package*>;
	qFinished = new std::queue<Package*>;
	packageCount = 0;
	id = -11;
	guid.first = -1;
	guid.second = -1;
	paGenerateRate = 0;
	nodeTime = 0;	
	cuRound = 0;
	float t_pSize = Config::getInstance()->getPackageSize();
	float t_bWidth = Config::getInstance()->getBandwidth();
	perTransDelay = t_pSize / t_bWidth;
	int maxRow = Config::getInstance()->getMaxRow()*Config::getInstance()->getMaxColumn();
	int maxColumn = Config::getInstance()->getMaxRow()*Config::getInstance()->getMaxColumn();
	routingMatrix = new d_matrix(maxRow, maxColumn);
	routingMatrix->initData(0);
	//shortPath = new vector<int>();
	shortRouting = new d_matrix(maxRow, maxColumn);
	shortRouting->initData(-1);
	trainRouting = new d_matrix(maxRow, maxColumn);
	trainRouting->initData(-1);
	m_outputCount = routingMatrix->getCol()*routingMatrix->getCol();
	outData = new double[m_outputCount];
	memset(outData, 0, m_outputCount * sizeof(outData));
	inData = nullptr;
	netQ = new NeuralNet[sqrt(m_outputCount)];
}


Node::~Node()
{
	if (qServe)
	{
		delete qServe;
	}
	if (netQ) {
		delete[] netQ;
	}
}

bool Node::isOuterNode() {
	if (outerNode) {
		return true;
	}
	else {
		return false;
	}
}

/*
void Node::inComingPackage(Package package) {

}
void Node::outPutPackage(Package pacage) {

}
*/
void Node::initInData(int size) {
	inData = new double[size];
	memset(inData, 0, size * sizeof(outData));
}

void Node::setId(int a, int b) {
	guid.first = a;
	guid.second = b;
	id = a * Config::getInstance()->getMaxColumn() + b;
}

void Node::initialPackage(){
	if (guid.first == 0 || guid.first == Config::getInstance()->getMaxRow() - 1 
		|| guid.second == 0 || guid.second == Config::getInstance()->getMaxColumn()-1) {
		outerNode = true;
		paGenerateRate = rand() % (int)Config::getInstance()->getMaxGenerateRate() + 1;
	}
	else {
		outerNode = false;
		//paGenerateRate = 0;
	}
	for (int i = 0; i < paGenerateRate; i++) {
		//generatePackage();
	}		

}

void Node::generatePaPerRound(vector<Node*>* outerNodes) {
	double pNumPer = (double)(Config::getInstance()->getBandwidth() / Config::getInstance()->getPackageSize());
	double gRatePerRound = Config::getInstance()->getMaxGenerateRate() / pNumPer;
	int threshold = rand()% 2000 * gRatePerRound;
	int ge_random = rand() % 1000;
	while (ge_random < threshold) {
		generatePackage(outerNodes);
		ge_random = ge_random + 1000;
	}
	nodeTime = nodeTime + perTransDelay;
}

void Node::generatePackage() {
	int pid = id * Config::getInstance()->gerMaxPacNumPerNode() + packageCount + 1;
	Package *m_package = new Package(pid, nodeTime);
	int dest = id;
	while (dest == id) {
		int dest1 = rand() % Config::getInstance()->getMaxColumn();
		int dest2 = (Config::getInstance()->getMaxRow() - 1) * Config::getInstance()->getMaxColumn() + rand() % Config::getInstance()->getMaxColumn();
		int dest3 = rand() % Config::getInstance()->getMaxRow() * Config::getInstance()->getMaxColumn();
		int dest4 = rand() % Config::getInstance()->getMaxRow() * Config::getInstance()->getMaxColumn() + Config::getInstance()->getMaxColumn() - 1;
		int randDest = rand() % 4;
		switch (randDest) {
		case 0:
			dest = dest1;
		case 1:
			dest = dest2;
		case 2:
			dest = dest3;
		case 3:
			dest = dest4;
		}
	}
	m_package->setDestination(dest);
	m_package->setGenerateTime(nodeTime);
	qServe->push(m_package);
	packageCount++;
}

void Node::generatePackage(vector<Node*>* outerNodes) {
	int pid = id * Config::getInstance()->gerMaxPacNumPerNode() + packageCount + 1;
	Package *m_package = new Package(pid, nodeTime);
	int dest = id;
	while (dest == id) {
		dest = outerNodes->at(rand() % outerNodes->size())->getId();
	}
	m_package->setDestination(dest);
	m_package->setGenerateTime(nodeTime);
	int size = Config::getInstance()->getMaxColumn()*Config::getInstance()->getMaxRow();
	m_package->setPathSize(size);
	for (int i = 0; i < size; i++) {
		m_package->getPathData(i) = trainRouting->getData(dest, i);
	}
	qServe->push(m_package);
	packageCount++;
}

Package* Node::outPackage() {
	Package* out_package = qServe->front();
	qServe->pop();
	return out_package;
}

void Node::inPackage(Package* in_package) {
	in_package->getHop()++;
	if (in_package->getDestination() == id) {
		if (in_package->getGenerateTime() == nodeTime) {	//this is to avoid the same round
			in_package->setTerminalTime(nodeTime + perTransDelay);
		}
		in_package->setTerminalTime(nodeTime);		
		qFinished->push(in_package);
	}
	else {
		qServe->push(in_package);
	}
}


void Node::saveNodeData(const char* name, int inDataSize, double* inData, bool clean, int dest = -1)
{
	char filename[30];
	char dir[20];
	int t_outputCount;
	m_pacNum = 0;
	allDelay = 0;
	allOnehopDelay = 0;
	if (dest == -1) {
		sprintf(filename, "%s%d%s", name, id, ".txt");
		t_outputCount = m_outputCount;
	}else{
		sprintf(filename, "%s%d%s%d%s",name, id, "/dest", dest, ".txt");
		sprintf(dir, "%s%d", name, id);
		if (_access(dir, 0) == -1) {
			_mkdir(dir);
		}
		t_outputCount = sqrt(m_outputCount);		
	}
	int t_inputCount = inDataSize;	
	FILE *fout = stdout;
	if (filename)
		if (clean) {
			fout = fopen(filename, "w+t");
			fprintf(fout, "input:");
			fprintf(fout, "%d", t_inputCount);
			fprintf(fout, "\t");
			fprintf(fout, "output:");
			fprintf(fout, "%d", t_outputCount);
			fprintf(fout, "\t");
			fprintf(fout, "testCount:");
			fprintf(fout, "%d", 0);
			fprintf(fout, "---------------------------------------\n");
		}
		else {
			fout = fopen(filename, "a+t");
		}		

		//routingMatrix->memcpyDataOut(outData, t_outputCount * sizeof(outData));

		for (int i = 0; i < t_inputCount; i++)
		{
			fprintf(fout, "%1.2f", inData[i]);
			fprintf(fout, "\t"); 
		}
		fprintf(fout, "toOut:");
		
		for (int i = 0; i < t_outputCount; i++)
		{
			if (dest == -1) {				
				fprintf(fout, "%1.2f", routingMatrix->getData(i));
			}
			else {
				if (Config::getInstance()->isSingleOutputMod()) {
					int p = shortRouting->getData(dest, 0);
					if (p == i) {
						fprintf(fout, "%1.2f", 1.00);
					}
					else {
						fprintf(fout, "%1.2f", 0.00);
					};
				}
				else {
					fprintf(fout, "%1.2f", routingMatrix->getData(dest, i));
				}
			}
			fprintf(fout, "\t");
		}		
		fprintf(fout, "\n");
		if (filename)
			fclose(fout);
		//delete[] outData;
}

void Node::calculateDelay(bool isTrained)
{
	char dir[20];
	char delayFilename[50];
	if (isTrained) {
		sprintf(delayFilename, "%s%d%s", "../TrainedDelay/delayOfDestination", id, ".txt");
		sprintf(dir, "%s", "../TrainedDelay");
		if (_access(dir, 0) == -1) {
			_mkdir(dir);
		}
	}
	else {
		sprintf(delayFilename, "%s%d%s", "../OSPFDelay/delayOfDestination", id, ".txt");
		sprintf(dir, "%s", "../OSPFDelay");
		if (_access(dir, 0) == -1) {
			_mkdir(dir);
		}
	}
	FILE *fout = stdout;
	if (delayFilename)
		fout = fopen(delayFilename, "w+t");
		fprintf(fout, "---------------------------------------\n");

		//routingMatrix->memcpyDataOut(outData, t_outputCount * sizeof(outData));
		fprintf(fout, "id");
		fprintf(fout, "\t");
		fprintf(fout, "initNode");
		fprintf(fout, "\t");
		fprintf(fout, "hop");
		fprintf(fout, "\t");
		fprintf(fout, "generateTime");
		fprintf(fout, "\t");
		fprintf(fout, "totalDeley");
		fprintf(fout, "\t");
		fprintf(fout, "oneHopDeley");
		fprintf(fout, "\n");

		Package* pac;
		while (!qFinished->empty())
		{
			pac = qFinished->front();
			m_pacNum++;
			allDelay = allDelay + pac->getDelay();
			float t_oneDelay = pac->getDelay() / pac->getHop();
			allOnehopDelay = allOnehopDelay + t_oneDelay;
			int t_nodeNum = floor(pac->getId() / Config::getInstance()->gerMaxPacNumPerNode());
			fprintf(fout, "%d", pac->getId());
			fprintf(fout, "\t");
			fprintf(fout, "%d", t_nodeNum);
			fprintf(fout, "\t");
			fprintf(fout, "%d", pac->getHop());
			fprintf(fout, "\t");
			fprintf(fout, "%d", pac->getGenerateTime());
			fprintf(fout, "\t");
			fprintf(fout, "%1.2f", pac->getDelay());
			fprintf(fout, "\t");
			fprintf(fout, "%1.2f", t_oneDelay);
			fprintf(fout, "\n");
			qFinished->pop();			
		}
		pac = nullptr;
		float averageDelay = allDelay / m_pacNum;
		float averageOnehopDelay = allOnehopDelay / m_pacNum;
		fprintf(fout, "total package number:");
		fprintf(fout, "%d", m_pacNum);
		fprintf(fout, "\n");
		fprintf(fout, "average delay:");
		fprintf(fout, "%1.2f", averageDelay);
		fprintf(fout, "\n");
		fprintf(fout, "average one-hop delay:");
		fprintf(fout, "%1.2f", averageOnehopDelay);
		fprintf(fout, "\n");
		if (delayFilename)
			fclose(fout);
}

string Node::toString(int a)
{
	char jF[32];
	sprintf(jF, "%d", a);
	string jFirst = jF;
	return jFirst;
}
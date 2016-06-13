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
}


Node::~Node()
{
	if (qServe)
	{
		delete qServe;
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

void Node::setId(int a, int b) {
	guid.first = a;
	guid.second = b;
	id = a * Config::getInstance()->getMaxColumn() + b;
}

void Node::initialPackage(){
	if (guid.first == 0 || guid.first == Config::getInstance()->getMaxRow() - 1 
		|| guid.second == 0 || guid.second == Config::getInstance()->getMaxColumn()-1) {
		outerNode = true;
		paGenerateRate = rand() % Config::getInstance()->getMaxGenerateRate() + 1;
	}
	else {
		outerNode = false;
		paGenerateRate = 0;
	}
	for (int i = 0; i < paGenerateRate; i++) {
		generatePackage();
	}		

}

void Node::generatePaPerRound(vector<Node*>* outerNodes) {
	float gRatePerRound = (float)Config::getInstance()->getMaxGenerateRate()/ (Config::getInstance()->getBandwidth() / Config::getInstance()->getPackageSize());
	float threshold = gRatePerRound;
	float ge_random = (rand() % 1000) / 1000.00;
	while (ge_random < threshold) {
		generatePackage(outerNodes);
		ge_random = ge_random + 1;
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

void Node::saveNodeData(const char* name, int maxOuterNum, double* inData, bool clean, int dest = -1)
{
	char filename[30];
	int t_outputCount;
	if (dest == -1) {
		sprintf(filename, "%s%d%s", name, id, ".txt");
		t_outputCount = m_outputCount;
	}else{
		sprintf(filename, "%s%d-%d%s",name, id, dest, ".txt");
		t_outputCount = sqrt(m_outputCount);
	}
	int t_inputCount = maxOuterNum;	
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
				fprintf(fout, "%1.2f", routingMatrix->getData(dest, i));
			}
			fprintf(fout, "\t");
		}		
		fprintf(fout, "\n");
		if (filename)
			fclose(fout);
		//delete[] outData;
}

void Node::calculateDelay()
{
	char delayFilename[50];
	sprintf(delayFilename, "%s%d%s", "../test/delayOfDestination", id, ".txt");
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
		int t_pac = 0;
		float allDelay = 0;
		float allOnehopDelay = 0;
		Package* pac;
		while (!qFinished->empty())
		{
			pac = qFinished->front();
			t_pac++;
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
		float averageDelay = allDelay / t_pac;
		float averageOnehopDelay = allOnehopDelay / t_pac;
		fprintf(fout, "total package number:");
		fprintf(fout, "%d", t_pac);
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
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

void Node::initialPackage() {
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

void Node::generatePaPerRound() {
	int ge_random = rand() % Config::getInstance()->getMaxGenerateRate();
	if (ge_random < paGenerateRate * perTransDelay) {
		generatePackage();
	}
	nodeTime = nodeTime + perTransDelay;
}

void Node::generatePackage() {
	int pid = id * 10000 + packageCount + 1;
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
	qServe->push(m_package);
	packageCount++;		
}

Package* Node::outPackage() {
	Package* out_package = qServe->front();
	qServe->pop();
	return out_package;
}

void Node::inPackage(Package* in_package) {
	if (in_package->getDestination() == id) {
		in_package->setTerminalTime(nodeTime);
		qFinished->push(in_package);
	}
	else {
		qServe->push(in_package);
	}
}

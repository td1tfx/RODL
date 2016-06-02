#include "stdafx.h"
#include "Node.h"


Node::Node()
{
	qServe = new std::queue<Package>;
	packageCount = 0;
	id = -11;
	guid.first = -1;
	guid.second = -1;
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

void Node::inComingPackage(Package package) {

}
void Node::outPutPackage(Package pacage) {

}

void Node::setId(int a, int b) {
	guid.first = a;
	guid.second = b;
	id = a * Config::getInstance()->getMaxColumn() + b;
}

void Node::initialPackage() {
	if (guid.first == 0 || guid.first == Config::getInstance()->getMaxRow() - 1 
		|| guid.second == 0 || guid.second == Config::getInstance()->getMaxColumn()-1) {
		outerNode = true;
	}
	else {
		outerNode = false;
	}
	if (outerNode) {
		int random = rand() % 100;
		for (int i = 0; i < random; i++) {
			generatePackage();
		}		
	}
	else {
		//temp
		generatePackage();
	}
}

void Node::generatePackage() {
	int pid = id * 10000 + packageCount;
	Package *m_package = new Package();
	int dest = id;
	while (dest == id) {
		int dest1 = rand() % Config::getInstance()->getMaxColumn();
		int dest2 = (Config::getInstance()->getMaxRow() - 1) * 10 + rand() % Config::getInstance()->getMaxColumn();
		int dest3 = rand() % Config::getInstance()->getMaxRow() * 10;
		int dest4 = rand() % Config::getInstance()->getMaxRow() * 10 + Config::getInstance()->getMaxColumn() - 1;
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
	qServe->push(*m_package);
	delete m_package;
	packageCount++;
		
}

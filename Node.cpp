#include "stdafx.h"
#include "Node.h"


Node::Node()
{
	qServe = new queue<Package>;
	if (guid.first == Config::getInstance()->getMaxRow() || guid.second == Config::getInstance()->getMaxColumn()) {
		outerNode = true;
	}
	else {
		outerNode = false;
	}
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
	id = a * 10 + b;
}

#include "stdafx.h"
#include "Topology.h"


Topology::Topology()
{
}


Topology::~Topology()
{
}

bool Topology::initGrid() {
	int nodeNum = Config::getInstance()->getMaxRow()*Config::getInstance()->getMaxColumn();
	m_nodes = new vector<Node>(nodeNum);
	m_nodes->resize(0);
	int val = m_nodes->capacity();
	for (int i = 0; i < Config::getInstance()->getMaxRow(); i++) {
		for (int j = 0; j < Config::getInstance()->getMaxColumn(); j++) {
			Node* node_t = new Node();
			node_t->setId(i, j);
			cout << "nodeId=" << node_t->getId() << endl;
			m_nodes->push_back(*node_t);
			delete node_t;
		}
	}
	if (m_nodes->begin() != m_nodes->end()) {
		return true;
	}
	else return false;

}

void Topology::initGraph() {
	if (initGrid()) {
	
	}
}

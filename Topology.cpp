#include "stdafx.h"
#include "Topology.h"


Topology::Topology()
{
}


Topology::~Topology()
{
	if (m_nodes) {
		delete m_nodes;
	}
}

bool Topology::initGrid() {
	//int nodeNum = Config::getInstance()->getMaxRow()*Config::getInstance()->getMaxColumn();
	m_nodes = new vector<Node*>;
	m_outerNodes = new vector<Node*>;
	//m_nodes->resize(0);
	for (int i = 0; i < Config::getInstance()->getMaxRow(); i++) {
		for (int j = 0; j < Config::getInstance()->getMaxColumn(); j++) {
			Node* node_t = new Node();
			node_t->setId(i, j);
			node_t->setPos(i * 10, j * 10);
			node_t->initialPackage();
			m_nodes->push_back(node_t);
			if (node_t->isOuterNode()) {
				Node* outerNode_t = new Node();
				*outerNode_t = *node_t;
				m_outerNodes->push_back(outerNode_t);
			}
			//delete node_t;
		}
	}
	if (m_nodes->begin() != m_nodes->end()) {
		return true;
	}
	else return false;

}

void Topology::initGraph() {
	if (initGrid()) {
		conGraph = new Graph;
		createNeighborGraph();
	}
}

void Topology::createNeighborGraph() {
	vector<Node*>::iterator i;
	for (i = m_nodes->begin(); i != m_nodes->end(); i++) {
		vector<Node*>::iterator j;
		for (j = m_nodes->begin(); j != m_nodes->end(); j++) {
			if (i == j) {
				continue;
			}
			else {
				if (getTwoNodesDistance(**i, **j) <= MAX_IR) {
					string iFirst = toString((*i)->getId());
					string jFirst = toString((*j)->getId());
					string linkEdge = iFirst + "->" + jFirst;
					Edge ed;
					ed = (add_edge((*i)->getId(), (*j)->getId(), *conGraph)).first;
					edges_index[ed] = linkEdge;
					edges_weight[ed] = (*j)->getPackageNum();
					cout << linkEdge << "; " << (*i)->getPos().first << "," << (*i)->getPos().second << 
						"; "<< (*j)->getPos().first << "," << (*j)->getPos().second << ", weight=" << (*j)->getPackageNum() << endl;
					
				}
			}
		}
	}
}


int Topology::getShortestPath(int destId) {

	Vertex s = vertex(destId, *conGraph);
	std::vector<Vertex> parent(num_vertices(*conGraph));
	std::vector<int> distMap(num_vertices(*conGraph));
	dijkstra_shortest_paths(*conGraph, s, predecessor_map(boost::make_iterator_property_map(parent.begin(), 
		get(boost::vertex_index, *conGraph))).distance_map(boost::make_iterator_property_map(distMap.begin(), get(boost::vertex_index, *conGraph))));

	//linkEdges = conGraph->getLinkTopology();
	//Link spLink;
	float sum = 0;

	float *miMetric = new float[m_nodes->size()];
	vector<Node*>::iterator i;
	for (i = m_nodes->begin(); i != m_nodes->end(); i++) {
		if ((*i)->getId() == destId) {
			miMetric[(*i)->getId()] = 0;
		}
		else {
			if (distMap.at((*i)->getId()) == NULL) {
				miMetric[(*i)->getId()] = 0;
			}
			else {
				string linkId;
				vector<int>* path = new vector<int>;
				int p = (*i)->getId();
				int num = 0;
				while (parent.at(p) != destId) {
					p = parent.at(p);
					(*i)->getRoutingMatrix()->getData(destId, p) = 1;
					(*i)->getParh()->at(destId)->at(num) = p;
					linkId = linkId + "->" + toString(p);
					num++;
				}
				//spLink = linkEdges->at(linkId);
				//miMetric[(*i)->getId()] = ;
				cout << "node=" << (*i)->getId() << "dest=" << destId << " dist=" + distMap[(*i)->getId()] << "path=" << linkId << endl;
			}
		}
	}

	for (int i = 0; i < sizeof(miMetric); i++) {
		sum += miMetric[i];
	}

	return sum;
}


float Topology::getTwoNodesDistance(int p1, int p2) {


	pair<float, float> posP1 = m_nodes->at(p1)->getPos();
	pair<float, float> posP2 = m_nodes->at(p2)->getPos();
	float distance = sqrt(pow(posP2.first - posP1.first, 2) + pow(posP2.second - posP1.second, 2));
	return distance;
}

float Topology::getTwoNodesDistance(Node &p1, Node &p2) {


	pair<float, float> posP1 = p1.getPos();
	pair<float, float> posP2 = p2.getPos();
	float distance = sqrt(pow(posP2.first - posP1.first, 2) + pow(posP2.second - posP1.second, 2));
	return distance;
}

string Topology::toString(int a)
{
	char jF[32];
	sprintf(jF, "%d", a);
	string jFirst = jF;
	return jFirst;
}
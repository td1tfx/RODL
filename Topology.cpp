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
	if (m_outerNodes) {
		delete m_outerNodes;
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
		inData = new double[m_outerNodes->size()];
		memset(inData, 0, m_outerNodes->size() * sizeof(inData));
		return true;
	}
	else return false;

}

void Topology::initGraph() {
	if (initGrid()) {
		conGraph = new Graph;
		cuTime = 0;
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
					edges_weight[ed] = (*j)->getPackageNum() + 1;
					//cout << linkEdge << "; " << (*i)->getPos().first << "," << (*i)->getPos().second << 
					//"; "<< (*j)->getPos().first << "," << (*j)->getPos().second << ", weight=" << (*j)->getPackageNum() << endl;
					
				}
			}
		}
	}
}

void Topology::updateNeighborGraph() {
	//std::cout << "edges(g) = ";
	edge_iter ei, ei_end;
	int weight;
	for (tie(ei, ei_end) = edges(*conGraph); ei != ei_end; ei++){
		weight = m_nodes->at(target(*ei, *conGraph))->getPackageNum();
		edges_weight[*ei] = weight + 1;
	}
}


bool Topology::getShortestPath(int destId) {

	Vertex s = vertex(destId, *conGraph);
	std::vector<Vertex> parent(num_vertices(*conGraph));
	std::vector<int> distMap(num_vertices(*conGraph));
	dijkstra_shortest_paths(*conGraph, s, predecessor_map(boost::make_iterator_property_map(parent.begin(), 
		get(boost::vertex_index, *conGraph))).distance_map(boost::make_iterator_property_map(distMap.begin(), get(boost::vertex_index, *conGraph))));

	//linkEdges = conGraph->getLinkTopology();
	//Link spLink;
	float sum = 0;

	vector<Node*>::iterator i;
	for (i = m_nodes->begin(); i != m_nodes->end(); i++) {
		if ((*i)->getId() == destId) {
		}
		else {
			if (distMap.at((*i)->getId()) == NULL) {				
				cout << "error!! Node=" << (*i)->getId() << "distMap = NULL, packageNum =" << (*i)->getPackageNum() << endl;
				return false;
			}
			else {
				vector<int>* path = new vector<int>;
				int p = (*i)->getId();
				(*i)->getRoutingMatrix()->getData(destId, p) = 1;
				string linkId = toString(p);
				string routingId = "";
				int num = 0;				
				while (p != destId){
					p = parent.at(p);
					(*i)->getRoutingMatrix()->getData(destId, p) = 1;
					(*i)->getShortPath()->getData(destId, num) = p;
					linkId = linkId + "->" + toString(p);
					num++;
				} 
				// print the path vactor
				//cout << "node=" << (*i)->getId() << ";dest=" << destId << ";dist=" << distMap[(*i)->getId()] << ";path=" << linkId << endl;
				/* print the routing vector
				for (int j = 0; j < m_nodes->size(); j++) {
					routingId = routingId + "->" + toString((*i)->getRoutingMatrix()->getData(destId, j));
				}
				cout << "node=" << (*i)->getId() << ";dest=" << destId << ";dist=" << distMap[(*i)->getId()] << ";path=" << routingId << endl;
				*/
			}
		}
	}
	return true;
}

//OSPF
void Topology::getAllShortestPath() {

	updateNeighborGraph();
	vector<Node*>::iterator i;
	for (i = m_nodes->begin(); i != m_nodes->end(); i++) {
		(*i)->getRoutingMatrix()->initData(0);
		(*i)->getShortPath()->initData(-1);
	}	
	for (i = m_outerNodes->begin(); i != m_outerNodes->end(); i++) {
		if (getShortestPath((*i)->getId())) {
			cout << "finished destination:" << (*i)->getId() << endl;
		}
	}
}

//use learning algorithm
void Topology::getAllTrainedPath() {

	vector<Node*>::iterator i;
	getOuterNodesLoad();
	totalPCount = 0;
	wrongPCount = 0;
	int inputGroupCount = 1;
	int outputNodeCount = m_outerNodes->at(0)->getOutputCount();
	auto t_output = new double[outputNodeCount*inputGroupCount];
	for (i = m_outerNodes->begin(); i != m_outerNodes->end(); i++) {
		(*i)->getRoutingMatrix()->initData(0);
		(*i)->getTrainPath()->initData(-1);
		(*i)->getNet().resetGroupCount(inputGroupCount);
		(*i)->getNet().InputNodeCount = m_outerNodes->size();
		(*i)->getNet().OutputNodeCount = outputNodeCount;
		//(*i)->getNet().setInputData(_train_inputData, InputNodeCount, 0);
		//(*i)->getNet().setInputData((*i)->getInData(), m_outerNodes->size(), 0);
		//(*i)->getNet().run();
		(*i)->getNet().activeOutputValue((*i)->getInData(), t_output, inputGroupCount);
		//cout << "node:" << (*i)->getId();
		for (int j = 0; j < outputNodeCount; j++) {
			if (t_output[j] > 0.5) {
				t_output[j] = 1;
			}
			else {
				t_output[j] = 0;
			}
			//cout << "-" << t_output[j];
		}
		//cout << endl;
		(*i)->getRoutingMatrix()->memcpyDataIn(t_output, outputNodeCount);
	}
	delete[] t_output;
	for (i = m_outerNodes->begin(); i != m_outerNodes->end(); i++) {
		getTrainedPath((*i)->getId());
	}
	cout << "totalPCount:" << totalPCount << ";wrongPCount" << wrongPCount << endl;
}

void Topology::getTrainedPath(int destId) {
	pair<vertex_iter, vertex_iter> vp;
	for (vp = vertices(*conGraph); vp.first != vp.second; ++vp.first) {
		Node* t_node = m_nodes->at(node_index[*vp.first]);
		if (node_index[*vp.first] == destId || !t_node->isOuterNode()) {
			//j
		}
		else {
			int p = node_index[*vp.first];
			Vertex pV = *vp.first;
			int sourceNode = p;
			string linkId = toString(sourceNode);
			adj_iter ai, ai_end;
			int num = 0;
			int count = 0;
			while (p != destId) {
				for (tie(ai, ai_end) = adjacent_vertices(pV, *conGraph); ai != ai_end; ++ai) {
					int nextNode = node_index[*ai];
					int isPath = t_node->getRoutingMatrix()->getData(destId, nextNode);
					if (isPath == 1) {
						p = node_index[*ai];
						pV = *ai;
						t_node->getTrainPath()->getData(destId, num) = p;
						linkId = linkId + "->" + toString(p);
						num++;
						break;
					}
					else {}
				}
				count++;
				if (count > m_nodes->size()) {
					linkId = toString(sourceNode);
					for (int i = 0; i < m_nodes->size(); i++) {
						p = t_node->getShortPath()->getData(destId, i);
						linkId = linkId + "->" + toString(p);
						t_node->getTrainPath()->getData(destId, i) = p;
						if (p == destId) { break; }
					}
					wrongPCount++;
				}
			}
			totalPCount++;
			cout << "node=" << node_index[*vp.first] << ";dest=" << destId << ";path=" << linkId << endl;
		}
	}
}
void Topology::runOneRound(){
	vector<Node*>::iterator i;
	for (i = m_nodes->begin(); i != m_nodes->end(); i++) {
		if((*i)->isOuterNode()){
			(*i)->generatePaPerRound(m_outerNodes);
		}
		if(!(*i)->isQueueEmpty()){
			Package* t_package = (*i)->outPackage();
			int t_dest = t_package->getDestination();
			int t_nextNodeId = (*i)->getNextNode(t_dest);
			Node* j = m_nodes->at(t_nextNodeId);
			j->inPackage(t_package);
			cuTime = (*i)->getNodeTime();
		}
	}
}

void Topology::runOneRoundWithTrain() {
	vector<Node*>::iterator i;
	for (i = m_nodes->begin(); i != m_nodes->end(); i++) {
		if ((*i)->isOuterNode()) {
			(*i)->generatePaPerRound(m_outerNodes);
		}
		if (!(*i)->isQueueEmpty()) {
			Package* t_package = (*i)->outPackage();
			int t_dest = t_package->getDestination();
			int t_nextNodeId = t_package->getNextNode();
			int t_sourceNodeId = (*i)->getId();
			if (t_nextNodeId<0 || t_nextNodeId>m_nodes->size() || getTwoNodesDistance((*i)->getId(), t_nextNodeId) > MAX_IR) {
				t_nextNodeId = (*i)->getNextNode(t_dest);
				cout << "traind path is wrong!" << endl;
			}
			Node* j = m_nodes->at(t_nextNodeId);
			j->inPackage(t_package);
			cuTime = (*i)->getNodeTime();
		}
	}
}

void Topology::runRounds(int num) {
	for (int i = 0; i < num; i++) {
		runOneRound();
	}
	//cout << "run round:" << num << " finisid!" << endl;
}

void Topology::runRoundsWithTrain(int num) {
	for (int i = 0; i < num; i++) {
		runOneRoundWithTrain();
	}
	//cout << "run round:" << num << " finisid!" << endl;
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


void Topology::getOuterNodesLoad() {
	vector<Node*>::iterator i;
	maxPackageNum = 1;
	int t_inputCount = m_outerNodes->size();
	for (i = m_outerNodes->begin(); i != m_outerNodes->end(); i++) {
		if ((*i)->getPackageNum() + 1 > maxPackageNum) {
			maxPackageNum = (*i)->getPackageNum() + 1;
		}
	}
	for (int i = 0; i < t_inputCount; i++) {
		double pkNum = m_outerNodes->at(i)->getPackageNum();
		double a = pkNum / maxPackageNum;
		inData[i] = a;
	}
	for (i = m_outerNodes->begin(); i != m_outerNodes->end(); i++) {
		if ((*i)->getInData() == nullptr) {
			(*i)->initInData(t_inputCount);
		}
		(*i)->setInData(inData, t_inputCount);
	}
}

void Topology::saveData(bool clean = false, const char* filename = "node", int dest = -1) {
	vector<Node*>::iterator i;	
	getOuterNodesLoad();
	int t_inputCount = m_outerNodes->size();
	for (i = m_outerNodes->begin(); i != m_outerNodes->end(); i++) {
		(*i)->saveNodeData(filename, t_inputCount, inData, clean, dest);
		int t_id = (*i)->getId();
		//cout << "node:" << t_id << "-data saved!" << endl;
	}
}

void Topology::saveDelay() {
	vector<Node*>::iterator i;
	for (i = m_outerNodes->begin(); i != m_outerNodes->end(); i++) {
		(*i)->calculateDelay();
	}
}

void Topology::readData(const char* filename) {
	int mark = 3;
	std::string str = readStringFromFile(filename) + "\n";
	if (str == "")
		return;
	std::vector<double> v;
	int n = findNumbers(str, v);
	int t_inputNodeCount = int(v[0]);
	int t_outputNodeCount = int(v[1]);

	int _train_groupCount = (n - mark) / (t_inputNodeCount + t_outputNodeCount);
	double* _train_inputData = new double[t_inputNodeCount * _train_groupCount];
	double* _train_expectData = new double[t_outputNodeCount * _train_groupCount];

	int k = mark, k1 = 0, k2 = 0;

	for (int i_data = 1; i_data <= _train_groupCount; i_data++)
	{
	for (int i = 1; i <= t_inputNodeCount; i++)
	{
		_train_inputData[k1++] = v[k++];
	}
	for (int i = 1; i <= t_outputNodeCount; i++)
	{
		_train_expectData[k2++] = v[k++];
	}
	}
}

void Topology::initTrainNet(int argc, char* argv[]) {
	vector<Node*>::iterator i;
	for (i = m_outerNodes->begin(); i != m_outerNodes->end(); i++) {
		if (argc > 1) {
			(*i)->getNet().loadOptoin(argv[1]);
		}
		else {
			(*i)->getNet().loadOptoin("learnConfig.ini");
		}
		(*i)->getNet().resetOption((*i)->getId());
		(*i)->getNet().init();
		cout << "node:" << (*i)->getId() << "NeuralNet init finished!" << endl;		
	}
}


int Topology::trainNet() {
	vector<Node*>::iterator i;
	for (i = m_outerNodes->begin(); i != m_outerNodes->end(); i++) {
		Timer t;
		t.start();
		(*i)->getNet().run();
		t.stop();
		fprintf(stderr, "node %d Run neural net end. Time is %lf s.\n", (*i)->getId(), t.getElapsedTime());
	}

#ifdef _WIN32
	getchar();
#endif
	return 0;
}

string Topology::toString(int a)
{
	char jF[32];
	sprintf(jF, "%d", a);
	string jFirst = jF;
	return jFirst;
}
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
				m_outerNodes->push_back(node_t);
			}
			//delete node_t;
		}
	}
	if (m_nodes->begin() != m_nodes->end()) {
		if (Config::getInstance()->isFullMod()) {
			inDataSize = m_nodes->size();
		}
		else {
			inDataSize = m_outerNodes->size();
		}
		inData = new double[inDataSize];
		memset(inData, 0, inDataSize * sizeof(inData));
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
	pair<vertex_iter, vertex_iter> vp;
	for (vp = vertices(*conGraph); vp.first != vp.second; ++vp.first) {
		Node* t_node = m_nodes->at(node_index[*vp.first]);
		int p = node_index[*vp.first];
		Vertex pV = *vp.first;
		adj_iter ai, ai_end;
		for (tie(ai, ai_end) = adjacent_vertices(pV, *conGraph); ai != ai_end; ++ai) {
			int nextNode = node_index[*ai];
			t_node->getNeigherNodes().push_back(nextNode);
		}
		t_node->getNodes() = m_nodes;
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
	for (i = m_nodes->begin(); i != m_nodes->end(); i++) {
		if (getShortestPath((*i)->getId())) {
			//cout << "finished destination:" << (*i)->getId() << endl;
		}
	}
	getNodesLoad();
}

//use learning algorithm
void Topology::getAllTrainedPath() {

	vector<Node*>::iterator i;
	getNodesLoad();
	totalPCount = 0;
	wrongPCount = 0;
	int inputGroupCount = 1;
	int outputNodeCount = m_outerNodes->at(0)->getOutputCount();
	auto t_output = new double[outputNodeCount*inputGroupCount];
	auto t_Soutput = new double[sqrt(outputNodeCount)*inputGroupCount];
	for (i = m_outerNodes->begin(); i != m_outerNodes->end(); i++) {
		(*i)->getRoutingMatrix()->initData(0);
		(*i)->getTrainPath()->initData(-1);
		if (Config::getInstance()->isSingleDestMod()) {
			if (Config::getInstance()->isSingleOutputMod()) {
				for (int j = 0; j < m_nodes->size(); j++) {
					Node* t_node = *i;
					if (m_nodes->at(j)->isOuterNode()) {
						totalPCount++;
					}					
					int count = 0;
					bool isbreak = false;
					int t_id = 0;
					while (t_node->getId() != j && isbreak == false) {
						count++;
						int pid = t_node->getId();
						t_node->getNet(j).resetGroupCount(inputGroupCount);
 						t_node->getNet(j).activeOutputValue((*i)->getInData(), t_Soutput, inputGroupCount);
						for (int n = 0; n < m_nodes->size(); n++) {
							int r = t_Soutput[n];
							if (t_Soutput[n] > 0.5) {
								(*i)->getTrainPath()->getData(j, t_id) = n;
								t_node = m_nodes->at(n);
								t_id++;
								break;
							}							
						}
						if (count > m_nodes->size()) {
							for (int q = 0; q < m_nodes->size(); q++) {
								int p = (*i)->getShortPath()->getData(j, q);
								(*i)->getTrainPath()->getData(j, q) = p;
								if (p == j) { break; }
							}
							if (m_nodes->at(j)->isOuterNode()) {
								wrongPCount++;
							}
							isbreak = true;
						}						
					}
				}
			}
			else {
				for (int j = 0; j < m_nodes->size(); j++) {
					(*i)->getNet(j).resetGroupCount(inputGroupCount);
					//(*i)->getNet(j).InputNodeCount = m_nodes->size();
					//(*i)->getNet(j).OutputNodeCount = outputNodeCount;
					(*i)->getNet(j).activeOutputValue((*i)->getInData(), t_Soutput, inputGroupCount);
					for (int n = 0; n < m_nodes->size(); n++) {
						if (t_Soutput[n] > 0.5) {
							(*i)->getRoutingMatrix()->getData(j, n) = 1;
						}
						else {
							(*i)->getRoutingMatrix()->getData(j, n) = 0;
						}
					}
				}
			}
		}
		else {
			(*i)->getNet().resetGroupCount(inputGroupCount);
			(*i)->getNet().activeOutputValue((*i)->getInData(), t_output, inputGroupCount);
			for (int m = 0; m < m_nodes->size(); m++) {
				for (int n = 0; n < m_nodes->size(); n++) {
					if (t_output[m + n*m_nodes->size()] > 0.5) {
						(*i)->getRoutingMatrix()->getData(m, n) = 1;
					}
					else {
						(*i)->getRoutingMatrix()->getData(m, n) = 0;
					}
				}
			}
		}
		/*
		cout << "node:" << (*i)->getId() << ":";
		for (int m = 0; m < m_nodes->size(); m++) {
		for (int n = 0; n < m_nodes->size(); n++) {
		cout << "-" << (*i)->getRoutingMatrix()->getData(m, n);
		}
		}
		cout << endl;
		*/
	}
	delete[] t_output;
	if (Config::getInstance()->isSingleOutputMod()) {
	}
	else {
		for (i = m_outerNodes->begin(); i != m_outerNodes->end(); i++) {
			getTrainedPath((*i)->getId());
		}
	}
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
			//cout << "node=" << node_index[*vp.first] << ";dest=" << destId << ";path=" << linkId << endl;
		}
	}
}

void Topology::runOneRound(){
	vector<Node*>::iterator i;
	float t_minTime = 9999999999;
	for (i = m_nodes->begin(); i != m_nodes->end(); i++) {
		if ((*i)->getNodeTime() > cuTime) {
			continue;
		}
		if((*i)->isOuterNode()){
			(*i)->generatePaPerRound(m_outerNodes);
		}
		if(!(*i)->isQueueEmpty()){
			Package* t_package = (*i)->outPackage();
			int t_dest = t_package->getDestination();
			int t_nextNodeId = (*i)->getNextNode(t_dest);
			Node* j = m_nodes->at(t_nextNodeId);
			j->inPackage(t_package, false);
			//float t_sigtime = (*i)->getPerTransSignalDelay();
			//float t_ptime = (*i)->getPerTransDelay();
			if (t_package->isSignaling()) {
				(*i)->getNodeTime() += (*i)->getPerTransSignalDelay();
			}
			else {
				(*i)->getNodeTime() += (*i)->getPerTransDelay();
			}			
		}else {
			(*i)->getNodeTime() += (*i)->getPerTransDelay();
		}
		if (t_minTime >= (*i)->getNodeTime()) {
			t_minTime = (*i)->getNodeTime();
		}
	}
	cuTime = t_minTime;
}

void Topology::runOneSignalRound(bool isTrained) {
	pair<vertex_iter, vertex_iter> vp;
	for (vp = vertices(*conGraph); vp.first != vp.second; ++vp.first) {
		Node* t_node = m_nodes->at(node_index[*vp.first]);
		int p = node_index[*vp.first];
		Vertex pV = *vp.first;
		adj_iter ai, ai_end;
		if (isTrained) {
			if (!t_node->isOuterNode()) {
				continue;
			}
			else {
				for (tie(ai, ai_end) = adjacent_vertices(pV, *conGraph); ai != ai_end; ++ai) {					
					int nextNode = node_index[*ai];
					if (!m_nodes->at(nextNode)->isOuterNode()) {
						continue;
					}
					else {
						t_node->generateSignaling(nextNode);
					}
				}
			}
		}
		else {
			for (tie(ai, ai_end) = adjacent_vertices(pV, *conGraph); ai != ai_end; ++ai) {
				int nextNode = node_index[*ai];
				t_node->generateSignaling(nextNode);
			}
		}
	}
}


void Topology::runOneRoundWithTrain() {
	vector<Node*>::iterator i;
	float t_minTime = 9999999999;
	for (i = m_nodes->begin(); i != m_nodes->end(); i++) {
		if ((*i)->getNodeTime() > cuTime) {
			continue;
		}
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
				//cout << "traind path is wrong!" << endl;
			}
			Node* j = m_nodes->at(t_nextNodeId);
			j->inPackage(t_package, true);
			if (t_package->isSignaling()) {
				(*i)->getNodeTime() += (*i)->getPerTransSignalDelay();
			}
			else {
				(*i)->getNodeTime() += (*i)->getPerTransDelay();
			}
		}
		else {
			(*i)->getNodeTime() += (*i)->getPerTransDelay();
		}
		if (t_minTime >= (*i)->getNodeTime()) {
			t_minTime = (*i)->getNodeTime();
		}
	}
	cuTime = t_minTime;
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


void Topology::getNodesLoad() {
	runOneSignalRound(isTrained);
	maxPackageNum = 1;
	vector<Node*>::iterator i;
	if (Config::getInstance()->isFullMod()) {
		for (i = m_nodes->begin(); i != m_nodes->end(); i++) {
			if ((*i)->getPackageNum() + 1 > maxPackageNum) {
				maxPackageNum = (*i)->getPackageNum() + 1;
			}
		}
		for (int i = 0; i < inDataSize; i++) {
			double pkNum = m_nodes->at(i)->getPackageNum();
			double a;
			if (!m_nodes->at(i)->isOuterNode()) {
				a = rand() % 1000 / 1000;
			}
			else {
				a = pkNum / maxPackageNum;
			}
			//a = pkNum / maxPackageNum;
			inData[i] = a;
		}
		for (i = m_nodes->begin(); i != m_nodes->end(); i++) {
			if ((*i)->getInData() == nullptr) {
				(*i)->initInData(inDataSize);
			}
			(*i)->setInData(inData, inDataSize);
		}
	}
	else {
		for (i = m_outerNodes->begin(); i != m_outerNodes->end(); i++) {
			if ((*i)->getPackageNum() + 1 > maxPackageNum) {
				maxPackageNum = (*i)->getPackageNum() + 1;
			}
		}
		for (int i = 0; i < inDataSize; i++) {
			double pkNum = m_outerNodes->at(i)->getPackageNum();
			double a = pkNum / maxPackageNum;
			inData[i] = a;
		}
		for (i = m_nodes->begin(); i != m_nodes->end(); i++) {
			if ((*i)->getInData() == nullptr) {
				(*i)->initInData(inDataSize);
			}
			(*i)->setInData(inData, inDataSize);
		}
	}
}

void Topology::saveData(bool clean = false, const char* filename = "node", int dest = -1) {
	vector<Node*>::iterator i;	
	getNodesLoad();
	if (Config::getInstance()->isSingleOutputMod()) {
		for (i = m_nodes->begin(); i != m_nodes->end(); i++) {
			if (Config::getInstance()->isSingleDestMod()) {
				for (int j = 0; j < m_nodes->size(); j++) {
					(*i)->saveNodeData(filename, inDataSize, inData, clean, j);
				}
			}
			else {
				(*i)->saveNodeData(filename, inDataSize, inData, clean, dest);
			}
			int t_id = (*i)->getId();
			//cout << "node:" << t_id << "-data saved!" << endl;
		}
	}
	else {
		for (i = m_outerNodes->begin(); i != m_outerNodes->end(); i++) {
			if (Config::getInstance()->isSingleDestMod()) {
				for (int j = 0; j < m_nodes->size(); j++) {
					(*i)->saveNodeData(filename, inDataSize, inData, clean, j);
				}
			}
			else {
				(*i)->saveNodeData(filename, inDataSize, inData, clean, dest);
			}
			int t_id = (*i)->getId();
			//cout << "node:" << t_id << "-data saved!" << endl;
		}
	}
}

void Topology::saveDelay(bool isTrained,double genarateRate) {
	char* DelayFile = "totalDelay.txt";
	FILE *fout = stdout;
	vector<Node*>::iterator i;
	int totalPacNum = 0;
	float totalDelay = 0;
	float totalOnehopDelay = 0;
	for (i = m_outerNodes->begin(); i != m_outerNodes->end(); i++) {
		(*i)->calculateDelay(isTrained);
		totalPacNum += (*i)->getFinalPacNum();
		totalDelay += (*i)->getAllDelay();
		totalOnehopDelay += (*i)->getAllOnehopDelay();
	}
	float averageDelay = totalDelay / totalPacNum;
	float averageOnehopDelay = totalOnehopDelay / totalPacNum;
	float throughputPerSecend = totalPacNum / cuTime;
	if (DelayFile){
		fout = fopen(DelayFile, "a+t");
		fprintf(fout, "ganerateRate = ");
		fprintf(fout, "%1.2f", genarateRate);
		fprintf(fout, "\n");
		fprintf(fout, "averageDelay = ");
		fprintf(fout, "%1.5f", averageDelay);
		fprintf(fout, "\n");
		fprintf(fout, "averageOnehopDelay = ");
		fprintf(fout, "%1.5f", averageOnehopDelay);
		fprintf(fout, "\n");
		fprintf(fout, "throughputPerSecend = ");
		fprintf(fout, "%1.5f", throughputPerSecend);
		fprintf(fout, "\n");
	}

	cout << "ganerateRate = " << genarateRate << endl;
	cout << "averageDelay = " << averageDelay << endl;
	cout << "averageOnehopDelay = " << averageOnehopDelay << endl;
	cout << "throughputPerSecend = " << throughputPerSecend << endl;

}

void Topology::saveWrongCount(bool clean)
{

	char* wrongCountFile = "wrongCount.txt";
	FILE *fout = stdout;
	if (wrongCountFile)
		if (clean) {
			fout = fopen(wrongCountFile, "w+t");
			fprintf(fout, "cuTime:");
			fprintf(fout, "\t");
			fprintf(fout, "totalPCount:");
			fprintf(fout, "\t");
			fprintf(fout, "wrongPCount");
			fprintf(fout, "\n");
			fprintf(fout, "---------------------------------------\n");
		}
		else {
			fout = fopen(wrongCountFile, "a+t");
			fprintf(fout, "%1.2f", cuTime);
			fprintf(fout, "\t");
			fprintf(fout, "%d", totalPCount);
			fprintf(fout, "\t");
			fprintf(fout, "%d", wrongPCount);
			fprintf(fout, "\n");
		}
	
	if (wrongCountFile)
		fclose(fout);
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
	if (Config::getInstance()->isSingleOutputMod()) {
		for (i = m_nodes->begin(); i != m_nodes->end(); i++) {
			if (Config::getInstance()->isSingleDestMod()) {
				for (int j = 0; j < m_nodes->size(); j++) {
					if (argc > 1) {
						(*i)->getNet(j).loadOptoin(argv[1]);
					}
					else {
						(*i)->getNet(j).loadOptoin("learnConfig.ini");
					}
					(*i)->getNet(j).resetOption((*i)->getId(), 1, j);
					(*i)->getNet(j).init();
					//cout << "node:" << (*i)->getId() << "-dest:" << j << "NeuralNet init finished!" << endl;
				}
			}
			else {
				if (argc > 1) {
					(*i)->getNet().loadOptoin(argv[1]);
				}
				else {
					(*i)->getNet().loadOptoin("learnConfig.ini");
				}
				(*i)->getNet().resetOption((*i)->getId());
				(*i)->getNet().init();
				//cout << "node:" << (*i)->getId() << "NeuralNet init finished!" << endl;
			}
		}
	}
	else {
		for (i = m_outerNodes->begin(); i != m_outerNodes->end(); i++) {
			if (Config::getInstance()->isSingleDestMod()) {
				for (int j = 0; j < m_nodes->size(); j++) {
					if (argc > 1) {
						(*i)->getNet(j).loadOptoin(argv[1]);
					}
					else {
						(*i)->getNet(j).loadOptoin("learnConfig.ini");
					}
					(*i)->getNet(j).resetOption((*i)->getId(), 1, j);
					(*i)->getNet(j).init();
					//cout << "node:" << (*i)->getId() << "-dest:" << j << "NeuralNet init finished!" << endl;
				}
			}
			else {
				if (argc > 1) {
					(*i)->getNet().loadOptoin(argv[1]);
				}
				else {
					(*i)->getNet().loadOptoin("learnConfig.ini");
				}
				(*i)->getNet().resetOption((*i)->getId());
				(*i)->getNet().init();
				//cout << "node:" << (*i)->getId() << "NeuralNet init finished!" << endl;
			}
		}
	}
	
}


int Topology::trainNet() {
	vector<Node*>::iterator i;
	if (Config::getInstance()->isSingleOutputMod()) {
		for (i = m_nodes->begin(); i != m_nodes->end(); i++) {
			Timer t;
			t.start();
			if (Config::getInstance()->isSingleDestMod()) {
				for (int j = 0; j < m_nodes->size(); j++) {
					(*i)->getNet(j).run();
				}
			}
			else {
				(*i)->getNet().run();
			}
			t.stop();
			fprintf(stderr, "node %d Run neural net end. Time is %lf s.\n", (*i)->getId(), t.getElapsedTime());
		}
	}
	else {
		for (i = m_outerNodes->begin(); i != m_outerNodes->end(); i++) {
			Timer t;
			t.start();
			if (Config::getInstance()->isSingleDestMod()) {
				for (int j = 0; j < m_nodes->size(); j++) {
					(*i)->getNet(j).run();
				}
			}
			else {
				(*i)->getNet().run();
			}
			t.stop();
			fprintf(stderr, "node %d Run neural net end. Time is %lf s.\n", (*i)->getId(), t.getElapsedTime());
		}
	}	

#ifdef _WIN32
	getchar();
#endif
	return 0;
}

int Topology::testNet() {
	vector<Node*>::iterator i;
	if (Config::getInstance()->isSingleOutputMod()) {
		for (i = m_nodes->begin(); i != m_nodes->end(); i++) {
			Timer t;
			t.start();
			if (Config::getInstance()->isSingleDestMod()) {
				for (int j = 0; j < m_nodes->size(); j++) {
					(*i)->getNet(j).runTest();
				}
			}
			else {
				(*i)->getNet().runTest();
			}
			t.stop();
			fprintf(stderr, "node %d Run neural net end. Time is %lf s.\n", (*i)->getId(), t.getElapsedTime());
		}
	}
	else {
		for (i = m_outerNodes->begin(); i != m_outerNodes->end(); i++) {
			Timer t;
			t.start();
			if (Config::getInstance()->isSingleDestMod()) {
				for (int j = 0; j < m_nodes->size(); j++) {
					(*i)->getNet(j).runTest();
				}
			}
			else {
				(*i)->getNet().runTest();
			}
			t.stop();
			fprintf(stderr, "node %d Run neural net end. Time is %lf s.\n", (*i)->getId(), t.getElapsedTime());
		}
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
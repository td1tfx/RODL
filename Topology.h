#pragma once

#include "Node.h"


using namespace std;

class Topology
{
private:
	
	const float MAX_IR = 13.26;
	Graph *conGraph;
	property_map<Graph, vertex_index_t>::type node_index = get(vertex_index, *conGraph);
	property_map<Graph, edge_index_t>::type edges_index = get(edge_index, *conGraph);
	property_map<Graph, edge_weight_t>::type edges_weight = get(edge_weight, *conGraph);
	vector<Node*>* m_nodes;
	vector<Node*>* m_outerNodes;
	float cuTime;
	int maxPackageNum;
	double* inData;
	int inDataSize;
	int totalPCount;
	int wrongPCount;

	void createNeighborGraph();
	void updateNeighborGraph();
	float getTwoNodesDistance(int p1, int p2);
	float getTwoNodesDistance(Node &p1, Node &p2);


public:
	Topology();
	~Topology();
	bool initGrid();
	void initGraph();
	string toString(int a);
	bool getShortestPath(int destId);
	void getTrainedPath(int destId);
	void getAllShortestPath();
	void getAllTrainedPath();
	void runOneRound();
	void runOneRoundWithTrain();
	void runRounds(int num);
	void runRoundsWithTrain(int num);
	void getNodesLoad();
	float getCuTime() {
		return cuTime;
	}
	void initTrainNet(int argc, char* argv[]);
	int trainNet();
	int testNet();

	void saveData(bool clean, const char* filename, int dest);
	void saveDelay(bool isTrained = false);
	void readData(const char* filename); 
	void saveWrongCount(bool clean);

};


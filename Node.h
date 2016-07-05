#pragma once
#include <vector>
#include <queue>
#include <iostream>
#include "Config.h"
#include "package.h"
#include <time.h>
#include "libconvert.h"
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/graph_traits.hpp"
#include "boost/graph/edge_list.hpp"
#include "boost/graph/dijkstra_shortest_paths.hpp"
#include "neural/MatrixFunctions.h"
#include "neural/NeuralNet.h"
#include "Timer.h"

using namespace std;
using namespace boost;

typedef property<edge_index_t, string, property<edge_weight_t, int>> EdgeProperty;
typedef property<vertex_index_t, int> VertexIndexProperty;
typedef adjacency_list<vecS, vecS, directedS, VertexIndexProperty, EdgeProperty> Graph;
typedef graph_traits<Graph>::edge_descriptor Edge;
typedef graph_traits<Graph>::edge_iterator edge_iter;
typedef graph_traits<Graph>::vertex_descriptor Vertex;
typedef graph_traits<Graph>::vertex_iterator vertex_iter;
typedef graph_traits<Graph>::out_edge_iterator out_edge_iter;
typedef graph_traits<Graph>::adjacency_iterator adj_iter;


class Node
{
private:
	float x, y;
	pair<float, float> pos;
	bool outerNode;
	pair<int, int> guid;
	int id;
	int cuRound;
	std::queue<Package*>* qServe;
	std::queue<Package*>* qFinished;
	std::queue<Package*>* qSFinished;
	float serveTime;
	int packageCount;
	int sPackageCount;
	int paGenerateRate;
	d_matrix* routingMatrix;
	d_matrix* shortRouting;
	d_matrix* trainRouting;
	float nodeTime;
	float perTransDelay;
	float perTransSignalDelay;
	double* inData;
	double* outData;
	int m_outputCount;
	int m_pacNum;
	float allDelay;
	float allOnehopDelay;
	NeuralNet net;
	NeuralNet* netQ;
	vector<int> neigherNodes;
	vector<Node*>* m_nodes;

public:
	Node();
	~Node();
	//void inComingPackage(Package package);
	//void outPutPackage(Package pacage);
	bool isOuterNode();
	void setId(int a, int b);
	vector<Node*>*& getNodes(){
		return m_nodes;
	}		

	int getId() {
		return id;
	}
	pair<int, int> getGuid(){
		return guid;
	}

	pair <float, float> getPos() {
		return pos;
	}
	NeuralNet& getNet() {
		return net;
	}

	NeuralNet& getNet(int i) {
		return netQ[i];
	}

	vector<int>& getNeigherNodes() {
		return neigherNodes;
	}

	double* getInData() {
		return inData;
	}

	void setInData(double* indata, int size) {
		memcpy(inData, indata, sizeof(double)*size);
	}

	void setPos(float x, float y) {
		pos.first = x;
		pos.second = y;
	}

	int getPackageNum() {
		return qServe->size();
	}

	float& getNodeTime() {
		return nodeTime;
	}
	float getPerTransDelay() {
		return perTransDelay;
	}
	int getOutputCount() {
		return m_outputCount;
	}

	d_matrix*& getRoutingMatrix() {
		return routingMatrix;
	}
	d_matrix*& getShortPath() {
		return shortRouting;
	}
	
	d_matrix*& getTrainPath() {
		return trainRouting;
	}

	bool isQueueEmpty() {
		return qServe->empty();
	}

	int getNextNode(int dest) {
		return shortRouting->getData(dest, 0);
	}

	int getFinalPacNum(){
		return m_pacNum;
	}

	int getFinalSPacNum() {
		return qSFinished->size();
	}

	float getAllDelay() {
		return allDelay;
	}
	float getAllOnehopDelay() {
		return allOnehopDelay;
	}
	float getPerTransSignalDelay() {
		return perTransSignalDelay;
	}

	void generatePackage();
	void generatePackage(vector<Node*>* outerNodes);
	void generateSignaling(int dest);
	void initialPackage();
	Package* outPackage();
	void inPackage(Package* in_package,bool istrained);
	void generatePaPerRound(vector<Node*>* outerNodes);
	void saveNodeData(const char* name, int inDataSize, double* inData, bool clean, int dest);
	void calculateDelay(bool isTrained = false);
	void initInData(int size);
	void broadcast();

	string toString(int a);
	

};


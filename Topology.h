#pragma once

#include "Node.h"
#include <iostream>


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

	void createNeighborGraph();
	float getTwoNodesDistance(int p1, int p2);
	float getTwoNodesDistance(Node &p1, Node &p2);

public:
	Topology();
	~Topology();
	bool initGrid();
	void initGraph();
	string toString(int a);
	bool getShortestPath(int destId);
	void getAllShortestPath();
};


#pragma once
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/graph_traits.hpp"
#include "boost/graph/edge_list.hpp"
#include "boost/graph/dijkstra_shortest_paths.hpp"
#include "Node.h"
#include <iostream>


using namespace std;
using namespace boost;

typedef property<edge_index_t, string, property<edge_weight_t, int>> EdgeProperty;
typedef property<vertex_index_t, int> VertexIndexProperty;
typedef adjacency_list<vecS, vecS, undirectedS, VertexIndexProperty, EdgeProperty> Graph;
typedef graph_traits<Graph>::edge_descriptor Edge;
typedef graph_traits<Graph>::edge_iterator edge_iter;
typedef graph_traits<Graph>::vertex_descriptor Vertex;
typedef graph_traits<Graph>::vertex_iterator vertex_iter;

class Topology
{
private:

	Graph *conGraph;
	property_map<Graph, vertex_index_t>::type node_index = get(vertex_index, *conGraph);
	property_map<Graph, edge_index_t>::type edges_index = get(edge_index, *conGraph);
	property_map<Graph, edge_weight_t>::type edges_weight = get(edge_weight, *conGraph);
	vector<Node>* m_nodes;

public:
	Topology();
	~Topology();
	bool initGrid();
	void initGraph();
};


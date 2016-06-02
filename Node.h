#pragma once
#include <vector>
#include <queue>
#include "Config.h"
#include "package.h"
#include <time.h>
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/graph_traits.hpp"
#include "boost/graph/edge_list.hpp"
#include "boost/graph/dijkstra_shortest_paths.hpp"
#include "neural/MatrixFunctions.h"

using namespace std;
using namespace boost;

typedef property<edge_index_t, string, property<edge_weight_t, int>> EdgeProperty;
typedef property<vertex_index_t, int> VertexIndexProperty;
typedef adjacency_list<vecS, vecS, directedS, VertexIndexProperty, EdgeProperty> Graph;
typedef graph_traits<Graph>::edge_descriptor Edge;
typedef graph_traits<Graph>::edge_iterator edge_iter;
typedef graph_traits<Graph>::vertex_descriptor Vertex;
typedef graph_traits<Graph>::vertex_iterator vertex_iter;

class Node
{
private:
	float x, y;
	pair<float, float> pos;
	bool outerNode;
	pair<int, int> guid;
	int id;
	std::queue<Package>* qServe;
	float serveTime;
	int packageCount;
	d_matrix* routingMatrix;
	typedef vector<int>* shortPath;
	vector<shortPath>* shortRouting;

public:
	Node();
	~Node();
	void inComingPackage(Package package);
	void outPutPackage(Package pacage);
	bool isOuterNode();
	void setId(int a, int b);
	int getId() {
		return id;
	}
	pair<int, int> getGuid(){
		return guid;
	}

	pair <float, float> getPos() {
		return pos;
	}

	void setPos(float x, float y) {
		pos.first = x;
		pos.second = y;
	}
	int getPackageNum() {
		return qServe->size();
	}

	d_matrix*& getRoutingMatrix() {
		return routingMatrix;
	}
	vector<shortPath>*& getParh() {
		return shortRouting;
	}

	void generatePackage();
	void initialPackage();
};


#pragma once
#include <vector>
#include <queue>
#include "Config.h"
#include "package.h"

using namespace std;

class Node
{
private:
	float x, y;
	bool outerNode;
	pair<int, int> guid;
	int id;
	queue<Package>* qServe;
	float serveTime;
	
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

};


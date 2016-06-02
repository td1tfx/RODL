#pragma once
#include "Config.h"

class Package
{
private:
	int id;
	int size;
	float delay;
	int destination;

public:
	Package();
	Package(int id);
	~Package();
	void setDestination(int dest) {
		destination = dest;
	}

};


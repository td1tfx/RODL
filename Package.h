#pragma once
#include "Config.h"

class Package
{
private:
	int id;
	int size;
	int delay;
	

public:
	Package();
	Package(int id);
	~Package();
};


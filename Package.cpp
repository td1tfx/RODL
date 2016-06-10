#include "stdafx.h"
#include "Package.h"


Package::Package()
{
}

Package::Package(int id, float time)
{
	this->id = id;
	size = Config::getInstance()->getPackageSize();
	generateTime = 0;
	terminalTime = 0;
	delay = 0;	
	hop = 0;
}


Package::~Package()
{
}

#include "stdafx.h"
#include "Package.h"


Package::Package()
{
}

Package::Package(int id, float time)
{
	this->id = id;
	size = Config::getInstance()->getPackageSize();
	delay = 0;	
}


Package::~Package()
{
}

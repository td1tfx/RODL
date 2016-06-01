#include "stdafx.h"
#include "Package.h"


Package::Package()
{

}

Package::Package(int id)
{
	this->id = id;
	size = Config::getInstance()->getPackageSize();
}


Package::~Package()
{
}

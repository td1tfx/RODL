#include "stdafx.h"
#include "Config.h"

Config* Config::m_config;


Config::Config()
{
	packageSize = 0;
	maxColumn = 10;
	maxRow = 10;
}


Config::~Config()
{
}

/*
void Config::init() 
{
}
*/

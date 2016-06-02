#include "stdafx.h"
#include "Config.h"

Config* Config::m_config;


Config::Config()
{
	packageSize = 0;
	maxColumn = 4;
	maxRow = 4;
}


Config::~Config()
{
}

/*
void Config::init() 
{
}
*/

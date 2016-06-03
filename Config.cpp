#include "stdafx.h"
#include "Config.h"

Config* Config::m_config;


Config::Config()
{
	packageSize = 512;
	maxColumn = 4;
	maxRow = 4;
	bandwidth = 1024;
	maxGenerateRate = 4;
}


Config::~Config()
{
}

/*
void Config::init() 
{
}
*/

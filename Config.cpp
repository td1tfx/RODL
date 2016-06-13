#include "stdafx.h"
#include "Config.h"

Config* Config::m_config;


Config::Config()
{
	packageSize = 256;
	maxColumn = 4;
	maxRow = 4;
	bandwidth = 1024;
	maxGenerateRate = 4;
	round = 10000;
	testRound = 100;
	maxPacNumPerNode = round;
	while (maxPacNumPerNode < round*maxGenerateRate) {
		maxPacNumPerNode = maxPacNumPerNode * 10;
	}	
}


Config::~Config()
{
}

/*
void Config::init() 
{
}
*/

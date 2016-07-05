// RODL.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Config.h"
#include <time.h>
#include "topology.h"

void getTrainData(Topology* topology, double geRate);
void getTestData(Topology* topology);
void runTrainedPath(Topology* topology, double geRate);


void main(int argc, char* argv[])
{
	for (int i = 0; i < 9; i++) {
		Topology* topology = new Topology;
		topology->initGraph();
		srand(time(NULL));
		double rate = 0.8 + i*0.1;
		Config::getInstance()->resetMaxGenerateRate(0.8 + i*0.1);
		topology->initTrainNet(argc, argv);		
		runTrainedPath(topology, rate);
		//getTrainData(topology, rate);
		//getTestData(topology);
		//topology->trainNet();
		//topology->testNet();
		delete topology;
	}
	system("pause");
}



void getTestData(Topology* topology) {

	char* filename = "../test/node";
	int dest = -1; //all data
	topology->isTrained = false;
	topology->getAllShortestPath();
	topology->runRounds(10);
	topology->saveData(true, filename, dest);
	int cuRound = 0;
	int maxRound = Config::getInstance()->getTestRound();
	while (cuRound < maxRound) {
		topology->runRounds(10);
		topology->getAllShortestPath();
		topology->saveData(false, filename, dest);
		cuRound++;
	}
	topology->saveDelay();

	cout << "test finalTime=" << topology->getCuTime() << endl;
}

void getTrainData(Topology* topology, double geRate) {
	char* filename = "../data/node";
	int dest = -1; //all data
	topology->isTrained = false;
	topology->getAllShortestPath();
	topology->runRounds(10);
	//topology->saveData(true, filename, dest);
	int cuRound = 0;
	int maxRound = Config::getInstance()->getRound();
	while (topology->getCuTime() < 10) {
		topology->runRounds(10);
		topology->getAllShortestPath();
		//topology->saveData(false, filename, dest);
		cuRound++;
	}
	topology->saveDelay(false, geRate);

	cout << "train finalTime=" << topology->getCuTime() << endl;
}

void runTrainedPath(Topology* topology, double geRate) {
	//char* filename = "../data/node";
	int dest = -1; //all data
	topology->isTrained = true;
	topology->getAllShortestPath();
	topology->getAllTrainedPath();
	topology->saveWrongCount(true);
	topology->runRoundsWithTrain(10);
	//topology->saveData(true, filename, dest);
	int cuRound = 0;
	int maxRound = Config::getInstance()->getRound();
	while (topology->getCuTime() < 10) {
		topology->runRoundsWithTrain(10);
		//topology->getAllShortestPath();
		topology->getAllTrainedPath();
		topology->saveWrongCount(false);
		//topology->saveData(false, filename, dest);
		cuRound++;
	}
	topology->saveDelay(true, geRate);

	cout << "train finalTime=" << topology->getCuTime() << endl;
}


// RODL.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Config.h"
#include <time.h>
#include "Topology.h"

void getTrainData(Topology topology);
void getTestData(Topology topology);
void runTrainedPath(Topology topology);


void main(int argc, char* argv[])
{
	Topology topology;
	topology.initGraph();
	srand(time(NULL));
	topology.initTrainNet(argc, argv);
	//runTrainedPath(topology);
	//getTrainData(topology);
	//getTestData(topology);
	topology.trainNet();
	//topology.testNet();
	system("pause");
}



void getTestData(Topology topology) {

	char* filename = "../test/node";
	int dest = -1; //all data
	topology.initGraph();
	topology.getAllShortestPath();
	topology.runRounds(10);
	topology.saveData(true, filename, dest);
	int cuRound = 0;
	int maxRound = Config::getInstance()->getTestRound();
	while (cuRound < maxRound) {
		topology.runRounds(10);
		topology.getAllShortestPath();
		topology.saveData(false, filename, dest);
		cuRound++;
	}
	topology.saveDelay();

	cout << "test finalTime=" << topology.getCuTime() << endl;
}

void getTrainData(Topology topology) {
	char* filename = "../data/node";
	int dest = -1; //all data
	topology.initGraph();
	topology.getAllShortestPath();
	topology.runRounds(10);
	topology.saveData(true, filename, dest);
	int cuRound = 0;
	int maxRound = Config::getInstance()->getRound();
	while (cuRound < maxRound) {
		topology.runRounds(10);
		topology.getAllShortestPath();
		topology.saveData(false, filename, dest);
		cuRound++;
	}
	//topology.saveDelay();

	cout << "train finalTime=" << topology.getCuTime() << endl;
}

void runTrainedPath(Topology topology) {
	//char* filename = "../data/node";
	int dest = -1; //all data
	topology.getAllShortestPath();
	topology.getAllTrainedPath();
	topology.saveWrongCount(true);
	topology.runRoundsWithTrain(10);
	//topology.saveData(true, filename, dest);
	int cuRound = 0;
	int maxRound = Config::getInstance()->getRound();
	while (cuRound < 100) {
		topology.runRoundsWithTrain(10);
		topology.getAllShortestPath();
		topology.getAllTrainedPath();
		topology.saveWrongCount(false);
		//topology.saveData(false, filename, dest);
		cuRound++;
	}
	//topology.saveDelay();

	cout << "train finalTime=" << topology.getCuTime() << endl;
}


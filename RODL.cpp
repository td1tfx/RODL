// RODL.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Config.h"
#include "neural/NeuralNet.h"
#include <time.h>
#include "Timer.h"
#include "Topology.h"

void getTrainData();
void getTestData();

int main(int argc, char* argv[])
{
	srand(time(NULL));
	//getTrainData();
	//getTestData();

	NeuralNet net;
	Timer t;

	if (argc > 1)
	{
		net.loadOptoin(argv[1]);
	}
	else
		net.loadOptoin("learnConfig.ini");

	t.start();
	net.run();
	t.stop();

	fprintf(stderr, "Run neural net end. Time is %lf s.\n", t.getElapsedTime());

#ifdef _WIN32
	getchar();
#endif
	return 0;
}

void getTestData() {

	char* filename = "../test/node";
	Topology topology;
	topology.initGraph();
	topology.getAllShortestPath();
	topology.saveData(true, filename);
	int cuRound = 0;
	int maxRound = Config::getInstance()->getTestRound();
	while (cuRound < maxRound) {
		topology.runRounds(10);
		topology.getAllShortestPath();
		topology.saveData(false, filename);
		cuRound++;
	}
	topology.saveDelay();

	cout << "test finalTime=" << topology.getCuTime() << endl;
}

void getTrainData() {
	char* filename = "../data/node";
	Topology topology;
	topology.initGraph();
	topology.getAllShortestPath();
	topology.saveData(true, filename);
	int cuRound = 0;
	int maxRound = Config::getInstance()->getRound();
	while (cuRound < maxRound) {
		topology.runRounds(10);
		topology.getAllShortestPath();
		topology.saveData(false, filename);
		cuRound++;
	}
	//topology.saveDelay();

	cout << "train finalTime=" << topology.getCuTime() << endl;
}
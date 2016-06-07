// RODL.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Config.h"
#include "neural/NeuralNet.h"
#include <time.h>
#include "Timer.h"
#include "Topology.h"

int main(int argc, char* argv[])
{
	srand(time(NULL));
	Topology topology;
	topology.initGraph();
	topology.getAllShortestPath();
	topology.saveData(true);
	topology.runRounds(100);
	topology.getAllShortestPath();
	topology.saveData(false);

	cout << "finalTime="<< topology.getCuTime() << endl;

	NeuralNet net;
	Timer t;

	if (argc > 1)
	{
		net.loadOptoin(argv[1]);
	}
	else
		net.loadOptoin("learnConfig.ini");

	t.start();
	//net.run();
	t.stop();

	fprintf(stderr, "Run neural net end. Time is %lf s.\n", t.getElapsedTime());

#ifdef _WIN32
	getchar();
#endif
	return 0;
}



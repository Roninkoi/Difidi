#include "sim.h"

using namespace std;

const string currentDateTime()
{
	auto now = chrono::system_clock::now();
	time_t timeNow = chrono::system_clock::to_time_t(now);
	return ctime(&timeNow);
}

int main(int argc, char *argv[])
{
	string inpath, outpath; // in/out files
	
	if (argc > 2) { // get paths of files from args
		inpath = argv[1];
		outpath = argv[2];
	}
	else {
		cout << "Not enough arguments! Specify the input and output files." << endl;
		return 1;
	}

	cout << "Diode finite differences, Roni Koitermaa 2021" << endl;
	cout << currentDateTime() << endl;
	cout << "infile: " << inpath << ", outfile: " << outpath << endl;
	
	Sim sim(inpath, outpath); // initialize simulator
	int result = sim.run(); // start main loop

	if (result == 0)
		cout << "Done!" << endl;
	
	return result; // sim return code (success, fail, ...)
}

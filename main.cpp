#include "Tuner.h"

int main(int argc, char * argv[])
{
	if(argc < 3){
		cerr << "Usage: ./ymert cands_file init_weights" << endl;
		return -1;
	}

	Tuner t(argv[1]);
	t.Run(argv[2]);
	return 0;
}

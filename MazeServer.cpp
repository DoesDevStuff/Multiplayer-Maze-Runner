//============================================================================
// Name        : MazeServer.cpp
// Author      : 
// Version     :
// Description : Maze Server
//============================================================================

#include "mazeServerController.h"

int main()
{
	//initializeController() does everything, main() only waits for termination flag
	if (!initializeController()) {
		cout << "Error starting server." << endl;
		return 1;
	}

	//do nothing
	while (!quitServerFlag) {
		Sleep(100);
	}

	Sleep(2000); //wait for threads to close

	quitServer();

	cout << "MazeServer: exiting" << endl;
	return 0;

}

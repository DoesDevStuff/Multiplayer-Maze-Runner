#include "tcpReceiver.h"

#include "mazeAddresses.h"
#include "mazeServerState.h"
#include "mazeServerController.h"

//sockets used for sending and receiving
SOCKET serverSocket;

// thread that listens for new connection from client
void tcpListenThread(void *param) {
	bool debug = true;

	if (debug) cout << "tcpListenThread started" << endl;

		while (true) {
			try {
				if (quitServerFlag ==true) { cout << "tcpListenThread: Quitting" << endl; return; }

				if ( listen(serverSocket, SOMAXCONN) == SOCKET_ERROR )
					cout << "tcpListenThread: listen failed" << endl;

				if (quitServerFlag ==true) { cout << "tcpListenThread: Quitting" << endl; return; }

				struct sockaddr receivedFrom; //discarded after use for now
				int receivedFromSize = sizeof(receivedFrom); //discarded after use for now
				if (debug) cout << "tcpListenThread: calling accept... waiting for new connection..." << endl;

				SOCKET newSocket = accept(serverSocket, &receivedFrom, &receivedFromSize);
				if (newSocket == INVALID_SOCKET) {
					cout << "invalid socket returned by accept" << endl;
					continue;
				}

				if (debug) cout << "tcpListenThread: new socket connected, calling processNewTcpConnection()" << endl;
				if (!processNewTcpConnection(newSocket)) {
					cout << "tcpListenThread: error processing new tcp connection, closing the new socket"<< endl;
					closesocket(newSocket);
				}
				else {
					if (debug) cout << "tcpListenThread: new socket processed successfully" << endl;
				}
			}
			catch(exception &ex) {
				cout << " tcpListenThread: exception: " << ex.what() << endl;
				continue;
			}
	}

}

//this thread is for receiving messages from client connections
//FYI: a thread is launched for each client --- to receive x and y coordinates
//This thread is passed a pointer to clientIndex so that a client can be
//quickly identified when there is incoming messages for ***optimization
void connectedClientTcpThread(void *clientIndexPtr) {
	bool debug = true;

	if (debug) cout << "connectedClientTcpThread started" << endl;

	int clientIndex = *(int *)clientIndexPtr;
	if ( (clientIndex < 0) || (clientIndex >= maxPlayers) ) {
		cout << "connectedClientTcpThread: bad clientIndex, exiting thread: " << clientIndex << endl;
		return;
	}

	//get MazePlayer from mazePlayers
	if (debug) cout << "connectedClientTcpThread: clientIndex=" << clientIndex << endl;
	MazePlayer mazePlayer = mazePlayers[clientIndex]; //get player at index

	while(true) {
		try {
			if (quitServerFlag ==true) { cout << "connectedClientTcpThread: Quitting" << endl; return; }

			int bytesReceived = recv(mazePlayer.tcpSocket, mazePlayer.tcpReceiveBuffer, maxTcpPacketSize, 0);
			if (bytesReceived <= 0)
				continue;

			if (quitServerFlag ==true) { cout << "connectedClientTcpThread: Quitting" << endl; return; }

			mazePlayer.tcpReceiveBuffer[bytesReceived] = 0;
			cout << "connectedClientTcpThread: new tcp message from client, clientIndex=" << clientIndex << ", Message: " << mazePlayer.tcpReceiveBuffer << endl;

			//return value ignored
			processTcpPlayerMovementMessage(clientIndex);
		}
		catch(exception &ex) {
			cout << " connectedClientTcpThread: exception: " << ex.what() << endl;
			continue;
		}
	}

}

bool initializeTcpListener() {
	cout << "initializeTcpListener: creating tcp socket..." << endl;
	//create server socket as tcp
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	cout << "initializeTcpListener: binding tcp socket..." << endl;
	if (bind(serverSocket, (sockaddr*)&tcpAddress, sizeof(tcpAddress)) < 0)
	{
		cout << "initializeTcpListener: Error binding socket to listen address" << endl;
		closesocket(serverSocket);
		return false;
	}

	cout << "initializeTcpListener: launching tcpListenThread" << endl;

	// start server thread let it start listening first .
	_beginthread(tcpListenThread, 0, NULL);

	return true;
}

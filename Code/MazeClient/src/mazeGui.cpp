#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include "common.h"
#include "mazeClientState.h"

using namespace sf;
using namespace std;

RenderWindow *sfmlWindow;
Vector2i player;
Vector2i opponent;
Vector2i endGate;

RectangleShape playerRect;
RectangleShape opponentRect;
RectangleShape endGateRect;

int *gameMap;
RectangleShape *displayRects; //The size of map

bool initializePlayers() {
	player = Vector2i(13, 13);  // player initialisation with position
	playerRect = RectangleShape(Vector2f(40.f, 40.f));
	playerRect.setPosition(player.x * 40.f, player.y * 40.f);
	playerRect.setFillColor(Color(0, 180, 0));
	playerRect.setOutlineThickness(1.f);
	playerRect.setOutlineColor(Color(0, 0, 0));

	opponent = Vector2i(2, 2);  // opponent initialisation with position
	opponentRect = RectangleShape(Vector2f(40.f, 40.f));
	opponentRect.setPosition(opponent.x * 40.f, opponent.y * 40.f);
	opponentRect.setFillColor(Color(180, 0, 0));
	opponentRect.setOutlineThickness(1.f);
	opponentRect.setOutlineColor(Color(0, 0, 0));

	endGate = Vector2i(1, 13);  // endGate initialisation with position
	endGateRect = RectangleShape(Vector2f(40.f, 40.f));
	endGateRect.setPosition(endGate.x * 40.f, endGate.y * 40.f);
	endGateRect.setFillColor(Color(0, 0, 180));
	endGateRect.setOutlineThickness(1.f);
	endGateRect.setOutlineColor(Color(0, 0, 0));

	playerX = player.x;
	playerY = player.y;
	opponentX = opponent.x;
	opponentY = opponent.y;

	gate_X = endGate.x;
	gate_Y = endGate.y;

	return true;
}

bool initializeGrid(int mapX, int mapY) {
	gameMap = new int[mapX * mapY];
	displayRects = new RectangleShape[mapX * mapY];

	// make visible grid
	for (int i = 0; i < 15; i++) {
		for (int j = 0; j < 15; j++) {
			displayRects[i + j * 15].setPosition(i * 40.f, j * 40.f);
			displayRects[i + j * 15].setSize(Vector2f(40.f , 40.f));
			displayRects[i + j * 15].setOutlineThickness(1.f);
			displayRects[i + j * 15].setOutlineColor(Color(0, 0, 0));

			if (!(i == player.x && j == player.y) && !(i == opponent.x && j == opponent.y) && !(i == endGate.x && j == endGate.y)) {
				// makes specific blocks black or rather spaces we can't go
				if (rand() / (float)RAND_MAX < 0.20f || i == 0 || j == 0 || i == 14 || j == 14) {
					gameMap[i + j * 15] = 1;
					displayRects[i + j * 15].setFillColor(Color(0, 0, 0));
				}
			}
		}
	}

	return true;
}

bool initializeSfAndPlayers(int width, int height, const char *title, int mapX, int mapY) {
	//create new SFML window
	sfmlWindow = new RenderWindow(VideoMode(width, height), title);
	if (sfmlWindow == NULL)
		return false;

	if (!initializePlayers())
		return false;

	if (!initializeGrid(mapX, mapY))
		return false;

	return true;
}

bool renderBoard() {
	//update coordinates from mazeClientState
	player.x = playerX;
	player.y = playerY;
	opponent.x = opponentX;
	opponent.y = opponentY;

	playerRect.setPosition(player.x * 40.f, player.y * 40.f); // set the new player position
	opponentRect.setPosition(opponent.x * 40.f, opponent.y * 40.f); // set the new chaser position

	sfmlWindow->clear(Color(255, 255, 255));

	for (int i = 0; i < 15 * 15; i++)
		sfmlWindow->draw(displayRects[i]);

	//draw Functions
	sfmlWindow->draw(playerRect);
	sfmlWindow->draw(opponentRect);
	sfmlWindow->draw(endGateRect);

	//display it
	sfmlWindow->display();

	return true;
}

bool initializeGui() {
	if (!initializeSfAndPlayers(600, 600, "Maze Game Chaos", 15, 15)) {
		return false;
	}

	renderBoard(); // render the board for the first time;

	return true;
}

//update newLocalX and newLocalY return true if there is a change in position
bool updateLocalXY(sf::Keyboard::Key eventKeyCode, Vector2i playerOrOpponent) {
	newLocalX = playerOrOpponent.x;
	newLocalY = playerOrOpponent.y;

	bool isChanged = true;
	if      ( (eventKeyCode == Keyboard::Up) && (gameMap[playerOrOpponent.x + (playerOrOpponent.y - 1) * 15] != 1) )    newLocalY -= 1;
	else if ( (eventKeyCode == Keyboard::Down) && (gameMap[playerOrOpponent.x + (playerOrOpponent.y + 1) * 15] != 1) )  newLocalY += 1;
	else if ( (eventKeyCode == Keyboard::Right) && (gameMap[(playerOrOpponent.x + 1) + playerOrOpponent.y * 15] != 1) ) newLocalX += 1;
	else if ( (eventKeyCode == Keyboard::Left) && (gameMap[(playerOrOpponent.x - 1) + playerOrOpponent.y * 15] != 1) )  newLocalX -= 1;
	else isChanged = false; // no change if none of the keys above are pressed

	return isChanged;
}

//handles key press events from SF and sets flags to send to server as well as to render locally
//TODO: too long function - split to avoid player and opponent in if-else
int doPlayerMovements() {
	Event event;

	if (!sfmlWindow->isOpen()) return -1; //quit if SF window is not open

	//check if there is any SF event, return 0 if there is no event
	if (!sfmlWindow->pollEvent(event)) return 0;

	//if close event is detected, close SF window and return -1 to quit
	if (event.type == Event::Closed) { sfmlWindow->close(); return -1; }

	//we are only interested if there are KeyPressed events, return 0 if event is not of type KeyPressed
	if (event.type != Event::KeyPressed) return 0;

	//get the code of the key pressed
	sf::Keyboard::Key eventKeyCode = event.key.code;

	// *** do not recognize any player moves till mazeClientState is mazeClientState_GameStarted
	if (mazeClientState != mazeClientState_GameStarted) {
		renderBoard(); //to refresh window
		return 0;
	}


	//if escape key is pressed, return -1 to quit
	if (eventKeyCode == Keyboard::Escape) return -1;

	Vector2i playerOrOpponent = (amIPlayer) ? player : opponent;
	//update newLocalX and newLocalY return true if there is a change in position
	bool changedFlag = updateLocalXY(eventKeyCode, playerOrOpponent);

	//return if no change
	if (!changedFlag)
		return 0;

	//double check --- x or y are not -ve
	if ( (newLocalX == -1) || (newLocalY == -1) )
		return 0;

	//	//there is a change with the keys pressed --- send this to server
	//	playerX = newLocalX;
	//	playerY = newLocalY;

	//if there is a change
	cout << ((amIPlayer) ? "Player" : "Opponent") << " Position changed..." << endl;
	//set flag positionsChangedLocally to make local change in parallel
	positionsChangedLocally = true;
	//set flag to send new positions to server
	sendNewPositionsToserver = true;

	return 1;
}

//This thread captures keystrokes, and triggers sending new positions to server (with TCP)
void playerMovementsThread(void *param) {
	int playermovement = 0; //less than zero = quit, 0=no movement, grater than zero = there is movement

	if (!initializeGui()) {
		cout << "initialization of Maze GUI failed" << endl;
		quitClient = true;
		return;
	}

	cout << "Maze GUI initialized" << endl;
	renderBoard(); // render the board for the first time;
	cout << "renderBoard() called" << endl;

	guiInitialized = true;

	//player movements loop
	while (true) {
		playermovement = doPlayerMovements(); //handles the SF keystroke events

		if (playermovement < 0) { // quit if < 0 --- //TODO: notify server that player wants to quit
			quitClient = true;
			break;
		}

/*
		if (playermovement == 0) // keep looping if there is no change
			continue;
*/

		//Note: two separate calls for rendering board as both server and local update can happen independently

		//to optimize, set the position in mazeClientState, and render GUI -- disabled for now
/*
		if (positionsChangedLocally) {
			if (amIPlayer) { playerX = newLocalX; playerY = newLocalY; }
			else { opponentX = newLocalX; opponentY = newLocalY; }

			positionsChangedLocally = false;
			renderBoard(); //comment out to test if movements are happening from server message
		}
*/

		//render GUI if positions changed from server update
		if (positionsChangedByServer) {
			positionsChangedByServer = false;
			renderBoard();
		}

		if (player == opponent) {
			cout << endl;
			cout << "------------------------------------------------------------" << endl;
			cout << "*** Opponent Wins, Game Over ***" << endl;
			cout << "------------------------------------------------------------" << endl;
			cout << endl;
			Sleep(1000); //for pause effect before quitting and to make sure server update is complete
			quitClient = true;
		}
		else if (player == endGate) {
			cout << endl;
			cout << "------------------------------------------------------------" << endl;
			cout << "*** Player Wins, Game Over ***" << endl;
			cout << "------------------------------------------------------------" << endl;
			cout << endl;
			Sleep(1000);  //for pause effect before quitting and to make sure server update is complete
			quitClient = true;
		}
	}
}

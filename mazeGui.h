#ifndef MAZEGUI_H_
#define MAZEGUI_H_

bool initializePlayers();
bool initializeGrid(int mapX, int mapY);
bool initializeSfAndPlayers(int width, int height, const char *title, int mapX, int mapY);

bool initializeGui();
int doPlayerMovements();
bool renderBoard();

void playerMovementsThread(void *param);


#endif /* MAZEGUI_H_ */

#ifndef MAZEMESSAGES_H_
#define MAZEMESSAGES_H_

//extern const char *

//json keys
extern const char *mazeMessageType;

extern const char *mmt_newPlayerJoinRequest; //join request from client with broadcast (UDP)

extern const char *mmt_newPlayerCalibration; //server sends calibration data, client sends back the same data
extern const char *mmvar_calibrationData;    //calibration data

//only when calibration is successful, mmvar_isAccepted will be true
extern const char *mmt_newPlayerServerResponse; //reply from server on same broadcast address (UDP)
extern const char *mmvar_isAccepted;
extern const char *mmvar_clientId;
extern const char *mmvar_isPlayer;
extern const char *mmvar_serverTcpAddress;
extern const char *mmvar_serverTcpPort;
extern const char *mmvar_serverMulticastAddress;
extern const char *mmvar_serverMulticastPort;

//message type sent by client when a client joins to server with Tcp (TCP - unicast - client > server)
extern const char *mmt_playerTcpConnection;
//const char *mmvar_clientID = "clientID"; //client ID
extern const char *mmvar_playerX; //X value in maze grid
extern const char *mmvar_playerY; //Y value in maze grid
extern const char *mmvar_gateX; //X value of gate in maze grid
extern const char *mmvar_gateY; //Y value of gate in maze grid

extern const char *mmt_playerCoordinates; //unicast (TCP) by client to server
//extern const char *mmvar_clientID;
//extern const char *mmvar_playerX;
//extern const char *mmvar_playerY;

extern const char *mmt_gameAndPlayerStatus; //multicast by server to all clients (game status, moves)
extern const char *mmvar_gameState;
//extern const char *mmvar_clientId;
//extern const char *mmvar_isPlayer;
extern const char *mmvar_playerX;
extern const char *mmvar_playerY;
extern const char *mmvar_playerState; //one of the values in mazeClientState (mazeClientState_xyz)

//hardcoded json values
extern const bool mmval_true;
extern const bool mmval_false;

#endif /* MAZEMESSAGES_H_ */

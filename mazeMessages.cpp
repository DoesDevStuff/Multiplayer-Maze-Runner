#include "mazeMessages.h"

//all maze messages are json strings on the wire
//nlohmann json library is used to parse and convert to json object
//so that values of different types can be extracted from the json object
//all messages should have a string variable "mazeMessageType"
//based on the message type value, code can interpret what is next

//*** json keys ***
//mandatory
const char *mazeMessageType = "mazeMessageType";

//all maze message types are prefixed with "mmt_"
//variables other than message types are prefixed with mmvar_

//mazeMessageTypes and variables in the type:
//-------------------------------------------
//message type sent by new player requesting to join maze game (UDP - broadcast client > all LAN)
const char *mmt_newPlayerJoinRequest = "newPlayerJoinRequest";
//variables in this message type: none

const char *mmt_newPlayerCalibration = "newPlayerCalibration"; //server sends calibration data, client sends back the same data
//variables in this message type:
const char *mmvar_calibrationData = "calibrationData";         //calibration data

//message type sent by server in response to join request (UDP - unicast server > client)
const char *mmt_newPlayerServerResponse = "newPlayerServerResponse";
//variables in this message type, expected values:
const char *mmvar_isAccepted = "isAccepted"; //true or false. if false, player is refused no other variables matter
const char *mmvar_clientId = "clientID"; //server generated numerical client ID to be used in all communications
const char *mmvar_isPlayer = "isPlayer"; //server assigns if the new joinee is player or chaser
//the following two are tcp details of server
const char *mmvar_serverTcpAddress = "serverTcpAddr"; //tcp listen address of server (string)
const char *mmvar_serverTcpPort = "serverTcpPort"; //tcp listen port of server (number)
//the following two are multicast details client to start listening on --- server will multicast clients
const char *mmvar_serverMulticastAddress = "multicastAddress"; //Multicast address - all clients should join
const char *mmvar_serverMulticastPort = "multicastPort"; //Multicast port - all clients should listen on

//message type sent by client only once when a client joins to server with Tcp (TCP - unicast - client > server)
//this is also to confirm that it is the same client previously joined with broadcast
//also, with this message, the player state is changed to "in lobby"
const char *mmt_playerTcpConnection = "playerTcpConnection";
//variables in this message type, expected values:
//const char *mmvar_clientID = "clientID"; //client ID
const char *mmvar_playerX = "playerX"; //X value in maze grid
const char *mmvar_playerY = "playerY"; //Y value in maze grid
const char *mmvar_gateX = "gateX"; //X value of gate in maze grid
const char *mmvar_gateY = "gateY"; //Y value of gate in maze grid

//message type sent by clients to send their coordinates to server (TCP - unicast - client > server)
const char *mmt_playerCoordinates = "playerCoordinates";
//const char *mmvar_clientID = "clientID"; //as generated above
//const char *mmvar_playerX = "playerX"; //X value in maze grid
//const char *mmvar_playerY = "playerY"; //Y value in maze grid

//message type sent by server to all clients --- game and player state, coordinates etc. (UDP - multicast - server > client)
//these messages control game proceedings --- the following messages will be sent in sequence
//mazeState_RequiredPlayersJoined, mazeState_BothPlayersInLobby, mazeState_GameStarted, mazeState_GameEnded;
//when both players join, server will send this message with gameState=not started - to set coordinates of both
//when game is in progress, server will send this message with gameState=in progress - to set coordinates of both
//when game ends, server will send this message with gameState=ended with coordinates, and who won or lost
const char *mmt_gameAndPlayerStatus = "gameAndPlayerState"; //type
const char *mmvar_gameState = "gameState"; //0=not started, 1=game in progress, 2=game ended
//const char *mmvar_clientId = "clientId"; //client ID of player
//const char *mmvar_isPlayer;
//const char *mmvar_playerX = "playerX"; //X location of player
//const char *mmvar_playerY = "playerY"; //Y location of player
const char *mmvar_playerState = "playerState"; //one of the values in mazeClientState (mazeClientState_xyz)

//*** hardcoded json values ***
const bool mmval_true = true;
const bool mmval_false = false;

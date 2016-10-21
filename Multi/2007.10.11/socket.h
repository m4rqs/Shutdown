#include "common.h"


char szRecvBuff[1024], szSendBuff[1024];
WSADATA	wsd;
SOCKET s, aSocket, wSocket;
struct sockaddr_in serveraddr;
struct sockaddr_in clientaddr;
WORD wEvent, wError;
int iSize, iError, iAddr;

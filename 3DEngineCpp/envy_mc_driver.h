#pragma once
#include <set>


void enableRegistration();
void disableRegistration();
DWORD WINAPI registerThread(LPVOID);
void sendCommand();
DWORD WINAPI responseFunnel(LPVOID param);
boolean newAddress(_int32* addr);
void sendAck(SOCKET sock);
DWORD WINAPI registerThread(LPVOID param);
void enableRegistration();
void disableRegistration();

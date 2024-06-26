#include<iostream>
#include<WinSock2.h>
#include<ws2tcpip.h>
#include<tchar.h>
#include<thread>
#include<vector>
using namespace std;

#pragma comment(lib, "ws2_32.lib")

/*
Intialize winsock library

Create the socket

get ip and port

bind the ip/port with the socket

listen on the socket

accept



*/

bool Intialize() {
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;

}

void InteractWithClient(SOCKET clientSocket, vector<SOCKET> clients){
	//send rec client

	cout << "Client connected" << endl;

	char buffer[4096];
	while (1) {

		int bytesrecv = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (bytesrecv <= 0) {
			cout << "client disconnected" << endl;
			break;
		}
		string message(buffer, bytesrecv);
		cout << "message from client : " << message << endl;

		for (auto client : clients) {
			if(client != clientSocket){
			send(client, message.c_str(), message.length(), 0);
			}
		}
	}

	auto it = find(clients.begin(), clients.end(), clientSocket);
	if (it != clients.end()){
		clients.erase(it);
	}

	closesocket(clientSocket);
}
int main() {
	if (!Intialize()) {
		cout << "winsock intialization failed" << endl;

	}

	cout << "server program" << endl;

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0); //1st para = addressing.AF_INET for IPv4, for IPv6 INET6 used, After define the protocol as a 2nd parameter here is TCP, 3rd parameter is 0

	if (listenSocket == INVALID_SOCKET) {
		cout << "Socket creation failed" << endl;
		return 1;
	}

	//create address structure
	int port = 88888; //it is a listening port
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(88888);

	//convert the ipaddress (0.0.0.0) put inside the sin_family in binary form
	if(InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1) {
		cout << "setting address structure failed" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//bind
	if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr))) {
		cout << "bind failed" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//Listen
	if (listen(listenSocket, SOMAXCONN)==SOCKET_ERROR) {
		cout << "listen failed" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	
	} //2nd para takes the value that can the requests in a queue, SOMAXCONN is sin integer
	cout << "server has started listening on port :" << port << endl;
	vector<SOCKET> clients;
	
	while (1) {
		//Accept
		SOCKET clientsocket = accept(listenSocket, nullptr, nullptr); // Client address as 2nd parameter, and print address as 2rd para, but not required so write null
		if (clientsocket == INVALID_SOCKET) {
			cout << "Invalid Socket" << endl;
		}
		clients.push_back(clientsocket);
		thread t1(InteractWithClient, clientsocket, std::ref(clients));
		t1.detach();
	}
	closesocket(listenSocket);
		
	WSACleanup();
	return 0;
}
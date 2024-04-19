#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<thread>
using namespace std;

#pragma comment(lib, "ws2_32.lib")


/*
initialize winsock

create socket

connect to the server

send/recv

close the socket

*/

bool Intialize() {
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}


void SendMess(SOCKET s) {
	cout << "Send your chat name : " << endl;
	string name;
	getline(cin, name);
	string message;


	while (1) {
		getline(cin, message);
		string msg = name + " : " + message;
		int bytesent = send(s, msg.c_str(), msg.length(), 0);
		if (bytesent == SOCKET_ERROR) {
			cout << "Error  in Sending Message " << endl;
			break;
		}
		if (message == "quit") {
			cout << "Stopping the Application" << endl;
		}

	}
	closesocket(s);
	WSACleanup();
}

void RecvMess(SOCKET s) {
	char buffer[4096];
	int recvlen;
	string msg = "";
	while (1) {
		recvlen = recv(s, buffer, sizeof(buffer), 0);
		if (recvlen <= 0) {
			cout << "Disconnected from the server" << endl;
			break;
		}
		else {
			msg = string(buffer, recvlen);
			cout << msg << endl;
		}
	}
	closesocket(s);
	WSACleanup();
}

int main() {
	if (!Intialize()) {
		cout << "Intialize winsock failed" << endl;

		return 1;
	}

	SOCKET s;
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET) {
		cout << "invalid socket created" << endl;
		return 1;
	}

	int port = 88888;
	string serveraddress = "127.0.0.1";
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	inet_pton(AF_INET, serveraddress.c_str(), &(serveraddr.sin_addr));

	if (connect(s, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
		cout << "not able to connect to server" << endl;
		cout << ": " << WSAGetLastError();
		closesocket(s);
		WSACleanup();
		return 1;
	}

	cout << "successfully connected to server" << endl;

	thread senderthread(SendMess, s);
	thread reciever(RecvMess, s);

	senderthread.join();
	reciever.join();
	return 0;

}

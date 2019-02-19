//server arch   |   client - server - client

#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

using namespace std;


int main() {

	//setup
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	//check for startup proper
	int wsOK = WSAStartup(ver, &wsData);
	if (wsOK != 0) { //bad start
		cerr << "Can't initialize WINSOCK! See ya" << endl;;
		return 0;
	}


	//socket creation. all hail the socket lord
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET) { //bad listener
		cerr << "Can't create socket. See ya" << endl;
		return 0;
	}


	//bind the socket. like a leather book
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000); //Host TO Network Short
	hint.sin_addr.S_un.S_addr = INADDR_ANY; //also use inet_pton

	bind(listening, (sockaddr*)&hint, sizeof(hint)); //binding the socket


	//tell winsock the socket is for listen
	listen(listening, SOMAXCONN); //max connection ~ 15


	//wait for connection
	//######TODO######
	//add more clients
	sockaddr_in client1;
	sockaddr_in client2;
	int client1size = sizeof(client1);
	int client2size = sizeof(client2);

	//can check for invalid socket
	SOCKET client1Socket = accept(listening, (sockaddr*)&client1, &client1size); //accept connection
	SOCKET client2Socket = accept(listening, (sockaddr*)&client2, &client2size); //accept connection

	char host1[NI_MAXHOST]; //client1's remote name
	char host2[NI_MAXHOST]; //client1's remote name
	char service[NI_MAXSERV]; //service client1 is connected on

	ZeroMemory(host1, NI_MAXHOST);
	ZeroMemory(host2, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	//check client1
	if (getnameinfo((sockaddr*)&client1, sizeof(client1), host1, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
		cout << host1 << " connected on port " << service << endl;
	}
	else {
		inet_ntop(AF_INET, &client1.sin_addr, host1, NI_MAXHOST);
		cout << host1 << " connected on port " << ntohs(client1.sin_port) << endl;
	}
	//check client2
	if (getnameinfo((sockaddr*)&client2, sizeof(client2), host2, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
		cout << host2 << " connected on port " << service << endl;
	}
	else {
		inet_ntop(AF_INET, &client2.sin_addr, host2, NI_MAXHOST);
		cout << host2 << " connected on port " << ntohs(client2.sin_port) << endl;
	}


	//close listening socket
	closesocket(listening);

	int timeout = 10000; //in milliseconds. this is 30 seconds
	

	//welcome message
	send(client1Socket, "Welcome to the server, Client 1\r\n", 38, 0);
	send(client2Socket, "Welcome to the server, Client 2\r\n", 38, 0);

	
	//message echoing
	char buff1[4096]; //up to 4096 characters
	char buff2[4096]; //up to 4096 characters
	int bytesRecieved1 = 0;
	int bytesRecieved2 = 0;

	while (true) {

		setsockopt(client1Socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)); //setting the receive timeout
		setsockopt(client2Socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)); //setting the receive timeout

		//reset
		ZeroMemory(buff1, 4096);
		ZeroMemory(buff2, 4096);

		//receiving
		bytesRecieved1 = recv(client1Socket, buff1, 4096, 0);
		bytesRecieved2 = recv(client2Socket, buff2, 4096, 0);

		//DC
		if ((bytesRecieved1 == 0) && (bytesRecieved2 == 0)) {
			cout << "Both clients disconnected." << endl;
			break;
		}
		if (bytesRecieved1 == 0) { //exit code. 'X' out
			cout << "Client 1 disconnected." << endl;
			break;
		}
		if (bytesRecieved2 == 0) { //exit code. 'X' out
			cout << "Client 2 disconnected." << endl;
			break;
		}

		//error checking
		if (bytesRecieved1 == SOCKET_ERROR) { //too much or something
			cerr << "Error in recv(). See ya" << endl;
			break;
		}
		if (bytesRecieved2 == SOCKET_ERROR) { //too much or something
			cerr << "Error in recv(). See ya" << endl;
				break;
		}


		//handling client messages
		//client 1
		if (bytesRecieved1 != 2) {
			cout << "Client 1: " << buff1 << endl; //repeat message to server

			//send to client2
			send(client2Socket, "Client 1: ", 10, 0);
			send(client2Socket, buff1, bytesRecieved1 + 1, 0); //echo message1
			send(client2Socket, "\r\n", 4, 0);
		}
		//client 2
		if (bytesRecieved2 != 2) {
			cout << "Client 2: " << buff2 << endl; //repeat message to server

			//send to client1
			send(client1Socket, "Client 2: ", 10, 0);
			send(client1Socket, buff2, bytesRecieved2 + 1, 0); //echo message1
			send(client1Socket, "\r\n", 4, 0);
		}
	}


	//close socket
	closesocket(client1Socket);
	closesocket(client2Socket);


	//cleanup winsock
	WSACleanup();



	return 0;
}

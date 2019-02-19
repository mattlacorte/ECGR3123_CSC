//client code   |   client - server

#include <iostream>
#include <string>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;


int main() {

	string ipAddy = "127.0.0.1"; // local machine
	int port = 54000; // listening port specified in server code


	// Initialize WinSock
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);

	if (wsResult != 0) {
		cerr << "Can't start Winsock. Err #" << wsResult << endl;
		return 0;
	}


	// create socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == INVALID_SOCKET) {
		cerr << "Can't create socket. Err #" << WSAGetLastError() << endl;
	}


	// hint struct
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddy.c_str(), &hint.sin_addr);


	//connect to server
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR) {
		cerr << "Can't connect to server. Err #" << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
		return 0;
	}

	




	// do while
	char buff[4096];
	string userInput;
	bool messageReceived = false;
	int bytesReceived = 0;
	int timeout = 10000; //in milliseconds. this is 3 seconds

	do {
		setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)); //setting the receive timeout

		// reset message check
		messageReceived = false;

		// check for initial message
		ZeroMemory(buff, 4096);
		bytesReceived = recv(sock, buff, 4096, 0);

		//################
		cout << "PASSED IT; TIMEOUT" << endl;

		// echo text to client
		if (bytesReceived > 0) {
			cout << "SERVER> " << string(buff, 0, bytesReceived) << endl;
			messageReceived = true;		// message received. Don't prompt
		}

		if (!messageReceived) {
			// prompt for text
			cout << "> ";
			getline(cin, userInput);

			if (userInput.size() > 0) { // make sure user typed
				// send text
				int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);

				// wait for response
				if (sendResult != SOCKET_ERROR) {
					ZeroMemory(buff, 4096);
					int bytesReceived = recv(sock, buff, 4096, 0);

					// echo text to client
					if (bytesReceived > 0) {
						cout << "SERVER> " << string(buff, 0, bytesReceived) << endl;
					}
				}
			}

			if (userInput == "gtfo") {
				break;
			}
		}
	} while ((userInput.size() > 0) || messageReceived);


	// close WinSock
	closesocket(sock);
	WSACleanup();

	return 0;
}



/* TranSec v1.1 */
/* Primary Control */

#include "ReceiverHeader.h"

WSADATA winsockData;
SOCKET inboundSocket;

using namespace std;
const int WinsockVersion = 2;

BOOL WINAPI consoleHandler(DWORD signal) { if (signal == CTRL_CLOSE_EVENT) { closesocket(inboundSocket); WSACleanup(); exit(0); return TRUE; } }

/* Gets the IP address of the computer */
string getIP() {
	/* Get the local hostname */
	char hostName[255];
	gethostname(hostName, 255);
	struct hostent *host_entry;
	host_entry = gethostbyname(hostName);

	/* Convert IP to a string */
	char* localIPAddress;
	localIPAddress = inet_ntoa(*(struct in_addr *)*host_entry->h_addr_list);
	string IPstring = localIPAddress;
	
	/* Cleanup Winsock */
	WSACleanup();
	return IPstring;
}

int runServer(short portNumber, string key) {
	bool fileAppend = FALSE; // set permission to append to the incoming file to false, so the header isn't added to the file
	string fileName;
	int packetcount = 1;
	int currentpacket = 0;
	vector<string> dataCollection;
	vector<pair<int, string>> allData;
	string dataString;
	size_t packet_location;
	string packet_text;
	int packet_number;
	vector<int> packets_received;

	string* dataArray = new string[1];

	const int bufferLength = 1024; // length of the socket buffer

	int sentCounter = 0;
	int returnResponse;

	cout << "Initializing receiving client:" << endl;
	cout << "  Retrieving IP..." << "\r" << flush; string IPstring = getIP(); cout << "  Receiving IP: " << IPstring << endl;
	cout << "  Receiving Port: " << portNumber << endl;
	cout << "  Encryption Key: " << key << endl << endl;

	if (WSAStartup(MAKEWORD(WinsockVersion, 0), &winsockData) == 0)
	{
		// Continue if the Winsock version matches
		if (LOBYTE(winsockData.wVersion) >= WinsockVersion)
		{

			/* Set up inbound socket */
			char in_buffer[bufferLength]; 
			char out_buffer[bufferLength];

			inboundSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  // create inbound socket
			if (inboundSocket == INVALID_SOCKET) { cout << "Error creating socket" << endl; }
			sockaddr_in inboundSocketAddress;
			inboundSocketAddress.sin_family = AF_INET;
			inboundSocketAddress.sin_port = htons(portNumber + 1); // +1 solely for encryptive purposes (although it likely doesn't matter)
			inboundSocketAddress.sin_addr.S_un.S_addr = INADDR_ANY;
			if (bind(inboundSocket, (sockaddr*)(&inboundSocketAddress), sizeof(inboundSocketAddress)) != 0) { cout << "Failed to bind to inbound socket"; }  // Bind to inbound socket
			if (listen(inboundSocket, SOMAXCONN) != 0) { cout << "Failed to listen to inbound socket"; _getch(); }  // Listen to inbound socket
			SOCKET inboundRemoteSocket;
			sockaddr_in remoteSocketAddr;
			int remoteSocketAddrLen = sizeof(remoteSocketAddr);

			/* Wait for connection on the inbound socket */
			cout << "Now waiting for connection..." << endl;
			inboundRemoteSocket = accept(inboundSocket, (sockaddr*)&remoteSocketAddr, &remoteSocketAddrLen);  // Accept incoming connections to a new, remote socket
			if (inboundRemoteSocket == INVALID_SOCKET) { cout << "Failed to accept incoming connections"; _getch(); return 0; }
			cout << "Connection established" << endl; // Connection has been established!

			/* "infinite" loop for packet collection */
			while (true)
			{
				int inboundResponse = recv(inboundRemoteSocket, in_buffer, sizeof(in_buffer) - 1, 0);  // waits for data to come, stores number of bytes read into inboundResponse
				if (inboundResponse == 0) { cout << endl << "[Error] Connection closed"; break; }  // connection must have been closed -- break
				/* Break program if error occurs while receiving */
				else if (inboundResponse == SOCKET_ERROR) { if (currentpacket - 1 == packetcount) { break; } cout << endl << "[Error] Socket error while receiving." << currentpacket; _getch(); return 0; }  // socket error -- break
				/* If everything is still stable... start sending the package */
				else
				{
					if (fileAppend == FALSE) { cout << "Now waiting for a file..." << "\r"; } // fileAppend is a signal for file content collection
					dataString.clear();
					dataString = in_buffer;
					/* Initial break conditions: if packet is the end flag ("E219") */
					if (dataString == "E219" && (currentpacket % packetcount == 0)) { cout << endl << "Received all file data!" << endl; fileAppend = FALSE; break; }
					if (dataString == "E219" && (currentpacket % packetcount != 0)) { cout << endl << "Failed to receive all packet data!  (" << currentpacket << " / " << packetcount << ") packets... " << endl; fileAppend = FALSE; _getch(); return 0; }
					/* otherwise, if permitted, append the data array with the packet contents */
					if (fileAppend == TRUE) {
						currentpacket++;
						dataArray[currentpacket - 1] = dataString;
					}

					/* If header is received, process it to get the file name and packet count; then permit entries into the data array */
					if (dataString.find("ts") != string::npos) {
						fileName = dataString.substr(dataString.find_first_of(">") + 1, dataString.find_last_of("<") - 6); // extract file name
						packetcount = atoi(dataString.substr(dataString.find_last_of("<") + 1, dataString.size()).c_str()); // extract packet count
						cout << endl << "Incoming file >" << fileName << "<" << endl;
						fileAppend = TRUE; // signals that file contents may now be received
						string* newArray = new string[packetcount];
						copy(dataArray, dataArray + min(1, packetcount), newArray);
						delete[] dataArray;
						dataArray = newArray;
					}

					cout << "Receiving file data:  " << currentpacket * 100 / packetcount << "%  ( " << currentpacket << " / " << packetcount << " )" << "\r" << flush;
				}
				memset(in_buffer, '\0', bufferLength); // resets inbound buffer
				memset(out_buffer, currentpacket, bufferLength); // loads received packet number to the outbound buffer, for confirmation
				returnResponse = send(inboundRemoteSocket, out_buffer, sizeof(out_buffer) - 1, 0);  // Sends confirmation
			}

			cout << "Processing package";

			int dataNumber;
			string dataPairString;

			vector<string> newDataVector(dataCollection.size());

			/* formats, decrypts, and assembles package */
			if (!dataArray) { dataArray = new string[1]; }
			for (int i = 0; i < packetcount; i++) { dataArray[i] = replace_chars_in_string(dataArray[i], ",", " ");  dataArray[i] = decrypt(dataArray[i], key); }
			for (int i = 0; i < packetcount; i++) {	newDataVector.push_back(dataArray[i]); }
			vector<int> packageIntVector = convertPackageToIntVector(newDataVector, key);

			cout << " -- complete" << endl;

			/* Write integer vectors to the file */
			ofstream fout;
			fout.open("./Received/" + fileName, ios::binary);
			for (int i : packageIntVector) {
				fout << (char)i;
			}
			fout.close();
			
			cout << "Received file written to " << "./Received/" + fileName << endl;
			cout << endl;
			cout << "Press any key to close this window";
			_getch();

		}
		else
		{
			// Winsock version wasn't found...
		}

		/* Cleanup Winsock */
		if (WSACleanup() != 0) { cout << endl << "\n[Error] Cleanup failed..." << endl; }
		closesocket(inboundSocket);
	}


	else
	{
		//  startup failed
	}
}

int main(int argc, char *argv[]) {

	if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
		printf("\n[Error] Could not set control handler");
		return 1;
	}

	cout << endl;
	cout << " - TranSec v1.1 - Receiver - (C) Jack Kolb, 2016-2017" << endl;
	cout << endl;

    /* Handles three value argument series: (1) no arguments provided, program asks, (2) all arguments provided, program starts, (3) all arguments provided, through GUI */

	if (argc != 1 && argc != 2 && argc != 3)
	{
		cout << "Invalid usage...\nCorrect Usage: TranSec_Receiver.exe [Port #] [Key]" << endl;
		cout << endl;
		cout << "Press any key to close this window";
		_getch();
		exit(1);
	}

	// No arguments given
	if (argc == 1)
	{
		short portNumber;
		cout << "Receiving Port #: "; cin >> portNumber;
		string key;
		cout << "Encryption Key: "; cin >> key; cout << endl;
		runServer(portNumber, key);
	}

	// Correct number of arguments given, through GUI
	if (argc == 2)
	{
		short portNumber = atoi(argv[0]);
		string key = argv[1];
		runServer(portNumber, key);
	}

	// Correct number of arguments given, through command line
	if (argc == 3)
	{
		short portNumber = atoi(argv[1]);
		string key = argv[2];
		runServer(portNumber, key);
	}
}

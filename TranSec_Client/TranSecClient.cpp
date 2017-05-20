/* TranSec v1.1 */
/* Controls the client program */

#include "TranSecClientHeader.h" // contains libraries and support functions

WSADATA wsaData;
SOCKET inboundSocket;
SOCKET outboundSocket;

using namespace std;

const int WinsockVersion = 2;
const int globalbuflen = 1024;
char recvbuf[globalbuflen];
const int recvbuflen = globalbuflen;

/* when the console is closed, cleans up Winsock */
BOOL WINAPI consoleHandler(DWORD signal) {
	if (signal == CTRL_CLOSE_EVENT) { closesocket(outboundSocket); }
	WSACleanup();
	exit(0);
	return TRUE;
}

/* sends the given string to the outbound socket */
int sendMessage(string message) {
	if (send(outboundSocket, message.c_str(), message.size(), 0) == SOCKET_ERROR) { return 1; }
	return 0;
}

/* listens on the outbound socket for a response */
int receiveMessage(int currentpacket) {
	memset(recvbuf, '\0', recvbuflen);
	int iResult = recv(outboundSocket, recvbuf, recvbuflen, 0);
	return 1;
}

/* primary client function */
int runClient(string receiverIP, short portNumber, string key, string fileLocationString) {

	/* initial display of inputted information */
	cout << endl;
	cout << "Initializing transmitting client:" << endl;
	cout << "  Target IP: " << receiverIP << endl;
	cout << "  Target Port: " << portNumber << endl;
	cout << "  Encryption Key: " << key << endl;
	cout << "  File Path: " << fileLocationString << endl;

	string fileName = isolateFileName(fileLocationString); // removes the path from the file path
	cout << "    File Name: " << fileName << endl << endl;

	/* prepare the package */
	cout << "Preparing transmission";
	vector<BYTE> hexVector = convertFileToHexVector(fileLocationString); // converts provided file path to a hexidecimal vector
	pair<vector<string>, vector<int>> overallPackagePair = convertHexVectorToPackage(hexVector, 100, key); // breaks the the hexidecimal vector into a shuffled packet vector
	vector<string> overallPackage = overallPackagePair.first; // first vector in the pair is the package
	vector<int> packetOrder = overallPackagePair.second; // second vector in the pair is the packet order

	/* encrypts all packets, replaces spaces with commas */
	string old_delimiter = " ";
	string new_delimiter = ",";
	for (int i = 0; i < overallPackage.size(); i++) {
		overallPackage[i] = encrypt(overallPackage[i], key);
		string::size_type n = 0;
		while ((n = overallPackage[i].find(old_delimiter, n)) != string::npos)
		{
			overallPackage[i].replace(n, old_delimiter.size(), new_delimiter);
			n += new_delimiter.size();
		}
	}
	cout << " -- complete" << endl; // package all prepared!

	int sentCounter = 0;
	int recint;

	/* startup Winsock */
	if (WSAStartup(MAKEWORD(WinsockVersion, 0), &wsaData) == 0) {
		/* set up outbound socket */
		cout << "Connecting to server";
		char out_buffer[globalbuflen];
		outboundSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (outboundSocket == INVALID_SOCKET) { cout << " -- Error creating socket" << endl; _getch(); return 0; }
		sockaddr_in outboundSocketAddress;
		outboundSocketAddress.sin_family = AF_INET;
		outboundSocketAddress.sin_port = htons(portNumber+1);
		outboundSocketAddress.sin_addr.S_un.S_addr = inet_addr(receiverIP.c_str());

		/* connect to outbound socket */
		if (connect(outboundSocket, (sockaddr*)(&outboundSocketAddress), sizeof(outboundSocketAddress)) != 0) { cout << "Failed to connect to outbound socket" << endl; _getch(); return 0; }
		cout << " -- complete" << endl;
		Sleep(200); // small pause before beginning

		/* initial header packet: file name, # of packets */
		int packetCounter = 0;
		string headerString = "ts1.0>" + fileName + "<" + to_string(overallPackage.size()) + "/" + to_string(packetCounter);
		sendMessage(headerString);
		sentCounter++;
		Sleep(1000); // short pause after header
		packetCounter++;
		string message;

		/* start sending packets, check after each one for a confirmation reply */
		for (int i : packetOrder) {
			message = overallPackage[i - 1];
			if (sendMessage(message) == 1) { cout << endl << "Failed to send data, press any key to close this window"; _getch; return 1; }
			sentCounter++;
			cout << "Transmitting file:  " << packetCounter * 100 / overallPackage.size() << "%" << "  ( " << packetCounter << " / " << overallPackage.size() << " )" << "\r" << flush;
			packetCounter++;
			receiveMessage(packetCounter);
			Sleep(5);
		}

		Sleep(1000); // short pause after package completes
		sendMessage("E219"); // "E219" signifies end of transmission
		sentCounter++;

		if (WSACleanup() != 0) { cout << endl << "cleanup failed..." << endl; } // cleans up WinSock
		closesocket(outboundSocket);

		cout << endl;
		cout << "Successfully transmitted the file!" << endl << endl;
		cout << "Press any key to close this window";
		_getch();
		return 1;
	}
	else
	{
		/* Failed to startup Winsock */
	}
}


int main(int argc, char *argv[]) {

	/* sets the console control handler (on exit, cleans up Winsock) */
	if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
		cout << endl << "[Error] Could not set control handler";
		cout << endl << "Press any key to close this window";
		_getch();
		return 1;
	}

	cout << endl;
	cout << " - TranSec v1.1 - Transmitter - (C) Jack Kolb, 2016-2017" << endl;
	cout << endl;

	/* Handles three argument formats: (1) no arguments given, so information is asked for (2) correct argument amount given, so program begins (3) program is run through the GUI */

	if (argc != 1 && argc != 4 && argc != 5) {
		cout << "Invalid usage...\nCorrect Usage: TranSec_Transmitter.exe [Target IP] [Target Port #] [Key] [File Path]" << endl;
		cout << endl;
		cout << "Press any key to close this window";
		_getch();
		exit(1);
	}

	/* No arguments provided, asks for each individually */
	if (argc == 1) {
		string receiverIP;
		cout << "Input receiving IP address: "; cin >> receiverIP;
		short portNumber;
		cout << "Input receiving port number: "; cin >> portNumber;
		string key;
		cout << "Input encryption key: "; cin >> key;
		string fileLocationNarrow;
		cout << "Input file path: "; cin >> fileLocationNarrow;

		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring fileLocationWide = converter.from_bytes(fileLocationNarrow);

		runClient(receiverIP, portNumber, key, fileLocationNarrow);
	}

	/* Correct number of arguments provided, through GUI */
	if (argc == 4)
	{
		string receiverIP = argv[0];
		short portNumber = atoi(argv[1]);
		string key = argv[2];
		string fileLocationNarrow = argv[3];

		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring fileLocationWide = converter.from_bytes(fileLocationNarrow);

		runClient(receiverIP, portNumber, key, fileLocationNarrow);
	}

	/* Correct number of arguments provided, through comand line */
	if (argc == 5)
	{
		string receiverIP = argv[1];
		short portNumber = atoi(argv[2]);
		string key = argv[3];
		string fileLocationNarrow = argv[4];

		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring fileLocationWide = converter.from_bytes(fileLocationNarrow);

		runClient(receiverIP, portNumber, key, fileLocationNarrow);
	}

	
}

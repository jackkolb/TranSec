/* TranSec v1.1*/
/* Header file connecting the support functions and primary control files */

#ifndef TRANSMITTERHEADER_H
#define TRANSMITTERHEADER_H

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <cstdio>
#include <conio.h>
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <Shlwapi.h>
#include <locale>
#include <codecvt>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <io.h>
#include <vector>
#include <bitset>
#include <istream>
#include <iterator>
#include <stdlib.h>
#include <stdio.h>
#include <utility>

#pragma comment(lib,"ws2_32.lib") //Winsock Library

using namespace std;

/* support functions used by the client function */
long getFileSize(FILE *file);
string hexToString(BYTE hexByte);
string isolateFileName(string filePath);
vector<int> convert_string_to_int_vector(string source);
vector<int> convert_key_to_int_vector(string source);
vector<int> convert_int_to_int_vector(int source);
bool vector_contains(const vector<int> &list, int x); // bool check for item's existence in a vector
string encrypt(string source, string key); 
vector<int> convertKeyToPacketOrder(string key);
vector<BYTE> convertFileToHexVector(string fileLocationString);
pair <vector<string>,vector<int>> convertHexVectorToPackage(vector<BYTE> &hexVector, int packetlength, string key);

#endif

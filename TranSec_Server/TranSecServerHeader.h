/* TranSec v1.1 */
/* Header File: Connects functions and libraries between ServerSupportFunctions.cpp and Server.cpp */

#ifndef TRANSECSERVERHEADER_H
#define TRANSECSERVERHEADER_H

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
#include <algorithm>
#include <array>

#pragma comment(lib,"ws2_32.lib") //Winsock Library

using namespace std;

vector<int> convert_string_to_int_vector(string source);
vector<int> convert_key_to_int_vector(string source);
vector<int> convert_int_to_int_vector(int source);
bool vector_contains(const vector<int> &list, int x);
string decrypt(string source, string key);
vector<int> convert_key_to_packet_order(string key);
vector<int> convertPackageToIntVector(vector<string> &dataVector, string key);
string replace_chars_in_string(string source, string old_char, string new_char);
bool compare(const pair<int, string>&i, const pair<int, string>&j);

#endif

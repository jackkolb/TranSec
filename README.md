#TranSec: An End-to-End-Encrypted File Transfer Program

##How to Use:

TranSec_Transmitter: Encrypts a file as per a key string, and sends to a port of a receiving IP address 

Command Line:
TranSec_Transmitter <receiving IPv4 address> <port number> <key> <file path>
If no arguments are provided, the program will ask for them individually


TranSec_Receiver: Listens on a port for an incoming file, then decrypts as per a key string and saves the file

Command Line:
TranSec_Receiver <port number> <key>
If no arguments are provided, the program will ask for them individually

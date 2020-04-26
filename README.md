# TranSec
An End-to-End-Encrypted File Transfer Program. This was my first C++ project. The program has two parts,
a receiver and a transmitter. The receiver listens at a specified port for a connection from a transmitter.
The transmitter "encrypts" a selected file and sends it to the receiver, who decodes it on the other end.
Effectively this allows for file sharing across a network.

The encryption algorithm used was entertaining to create and the best I could invent given absolutely no
background in cybersecurity. The given key is used to generate a series of bitwise "shuffles", "swaps",
and "shifts" to scramble the data. The data is sent to the receiver, which uses the key to unscramble the
data. I can make absolutely no guarantees to the effectiveness of this algorithm.

## How to Use:

### TranSec_Transmitter
Encrypts a file as per a key string, and sends to a port of a receiving IP address.

Command Line:
TranSec_Transmitter \<receiving IPv4 address\> \<port number\> \<key\> \<file path\>

If no arguments are provided, the program will ask for them individually


### TranSec_Receiver
Listens on a port for an incoming file, then decrypts as per a key string and saves the file,

Command Line:
TranSec_Receiver \<port number\> \<key\>

If no arguments are provided, the program will ask for them individually


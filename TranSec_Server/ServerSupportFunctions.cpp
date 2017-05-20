/* TranSec v1.1 */
/* Support Methods: decryption (and all required decryption techniques), conversions, etc. */

#include "OverallHeader.h"

using namespace std;


bool compare(const pair<int, string>&first, const pair<int, string>&second) { return second.first > first.first; }
int key_to_vector(string key, int iteration) { return 0; }
bool vector_contains(const vector<int> &list, int x) { return find(list.begin(), list.end(), x) != list.end(); }

// replaces all instances of a character
string replace_chars_in_string(string source, string old_char, string new_char) {
	string::size_type n = 0;
	while ((n = source.find(old_char, n)) != string::npos)
	{
		source.replace(n, old_char.size(), new_char);
		n += new_char.size();
	}
	return source;
}

// takes key, splits it into 1s and 0s, and uses that to make the packet order
vector<int> convert_key_to_packet_order(string key) {
	vector<int> keyBinaryArray;
	int keyCodeCounter = 1;
	for (int keyChar : key) {
		bitset<sizeof(char) * CHAR_BIT> binary(keyChar);
		string binaryString = binary.to_string();
		vector<int> binaryArray;
		for (char& i : binaryString) {
			binaryArray.push_back(i);
		}

		int binArraySize = binaryArray.size(); // retrieves size of binary array
		for (int i = 0; i < binArraySize; i++) { // subtracts 48 from binary array as correction
			binaryArray[i] -= 48;
		}

		for (int i = 0; i < binArraySize; i++) {
			if (binaryArray[i] == 1) {
				binaryArray[i] = keyCodeCounter;
				keyCodeCounter++;
			}
		}
		for (int i = 0; i < binArraySize; i++) {
			if (binaryArray[i] == 0) {
				binaryArray[i] = keyCodeCounter;
				keyCodeCounter++;
			}
		}

		keyBinaryArray.insert(end(keyBinaryArray), begin(binaryArray), end(binaryArray));
	}
	return keyBinaryArray;
}

vector<int> convert_string_to_int_vector(string source) {
	vector<string> string_vector;
	vector<int> int_vector;
	istringstream iss(source);
	for (string source; iss >> source; ) { string_vector.push_back(source); }
	for (auto &s : string_vector) { stringstream parser(s); int x = 0; parser >> x; int_vector.push_back(x); }
	return int_vector;
}

vector<int> convert_key_to_int_vector(string source) {
	vector<int> int_vector;
	for (char& c : source) { int_vector.push_back(c); }
	return int_vector;
}

vector<int> convert_int_to_int_vector(int source) {
	vector<int> int_vector;
	string source_string = to_string(source);
	for (int i = 0; i < source_string.size(); i++) { int_vector.push_back((int)(source_string.at(i)) - 48); }
	return int_vector;
}

vector<int> convert_int_vector_to_scramble_sequence(vector<int> &source, int number_of_scramble_types) {
	vector<int> sequence_vector;
	int scramble_type_split = (int)1 * 1000 / number_of_scramble_types;
	for (int i : source) {
		int raw_prep = (i*i*i / 2.19);
		int raw = raw_prep % 1000;
		for (int i = 1; i <= number_of_scramble_types; i++) { if (raw >= scramble_type_split * (i - 1) && raw < scramble_type_split * i) { sequence_vector.push_back(i); } }
	}
	return sequence_vector;
}

vector<string> unscramble_package(vector<string> &package, vector<int> packetOrder) {
	vector<string> newPackage;
	for (int i = 0; i < package.size(); i++) { newPackage.push_back(""); }
	for (int i = 0; i < packetOrder.size(); i++) { newPackage[packetOrder[i]-1] = package[i]; }
	return newPackage;
}

vector<int> convertPackageToIntVector(vector<string> &dataVector, string key) {
	int fullpacketcount = dataVector.size();
	int packetcount = fullpacketcount;
	vector<int> packetOrder = convert_key_to_packet_order(key);
	int packetOrderRemovalPosition = 0;
	vector<int> tempvector;
	for (int i : packetOrder) { if (i <= packetcount) { tempvector.push_back(i); } }
	packetOrder = tempvector;
	vector<int> addpacketorder = packetOrder;
	int packetOrderSize = packetOrder.size();
	while (packetcount > packetOrder.size()) {
		for (int i : addpacketorder) { i += packetOrderSize; if (i <= packetcount) { packetOrder.push_back(i); } if (packetOrder.size() == packetcount) { break; } }
		packetOrderSize = packetOrder.size();
	}
	vector<string> unscrambled_dataVector = unscramble_package(dataVector, packetOrder);
	vector<int> intVector;
	vector<int> packetIntVector;
	for (string i : unscrambled_dataVector) {
		packetIntVector = convert_string_to_int_vector(i);
		for (int i : packetIntVector) {	intVector.push_back(i);	}
	}
	return intVector;
}

unsigned int convert_key_to_int(string key) {
	vector<char> key_char_array(key.c_str(), key.c_str() + key.size() + 1);
	unsigned int key_as_int = 0;
	for (char i : key_char_array) { key_as_int += (int)i; }
	for (int i = 1; i <= 2; i++) { key_as_int *= key_as_int; }
	key_as_int = key_as_int % 2198239976;
	return key_as_int;
}


vector<int> convert_int_to_rand_int_vector(int seed) {
	vector<int> int_vector;
	seed = seed*seed;
	vector<int> seed_int_vector = convert_int_to_int_vector(seed);
	unsigned int rand_seed = seed * 13;
	vector<int> rand_seed_vector = convert_int_to_int_vector(rand_seed);
	vector<int> one_two_vector;
	for (int i : rand_seed_vector) { if (i > 4) { one_two_vector.push_back(2); } else { one_two_vector.push_back(1); } }
	int final_digit;
	int first_digit;
	int second_digit;
	for (int i = 0; i < one_two_vector.size() && i < seed_int_vector.size() - 1; i++) {
		final_digit = 0;
		first_digit = seed_int_vector[i];
		if (one_two_vector[i] == 2) { second_digit = seed_int_vector[i + 1]; final_digit = first_digit * 10 + second_digit; }
		else { final_digit = first_digit; }
		int_vector.push_back(final_digit);
	}
	for (int i = 0; i < int_vector.size(); i++) { if (int_vector[i] < 0) { int_vector[i] = int_vector[i] * -1; } }
	return int_vector;
}

vector<int> convert_int_seed_to_custom_length_vector(int seed, int length) {
	vector<int> rand_vector;
	rand_vector.reserve(length);
	seed += 13;
	seed = seed * seed * seed;
	unsigned int new_seed;
	vector<int> custom_length_vector = convert_int_to_int_vector(seed);
	vector<int> new_seed_vector;
	int digit_one;
	int digit_two;
	int new_iteration;
	int digit_counter = 0;
	if (custom_length_vector.size() % 2 != 0) { custom_length_vector.pop_back(); }
	while (custom_length_vector.size() <= length) {
		digit_one = custom_length_vector[digit_counter];
		digit_two = custom_length_vector[digit_counter + 1];
		digit_counter += 2;
		new_iteration = digit_one * 10 + digit_two + 17;
		new_seed = new_iteration * new_iteration * (new_iteration / 2);
		new_seed_vector = convert_int_to_rand_int_vector(new_seed);
		for (int i : new_seed_vector) { custom_length_vector.push_back(i); }
	}
	while (custom_length_vector.size() > length) { custom_length_vector.pop_back(); }
	return custom_length_vector;
}

string de_char_shuffle(string source, int iteration) {
	iteration += 11;
	string result = "";
	vector<int> source_int_vector = convert_string_to_int_vector(source);
	int source_int_vector_length = source_int_vector.size();
	unsigned int seed = iteration * iteration * iteration;
	unsigned int new_seed;
	vector<int> shuffle_vector = convert_int_to_int_vector(seed);
	vector<int> new_seed_vector;
	int digit_one;
	int digit_two;
	int new_iteration;
	vector<int> seed_vector;
	while (shuffle_vector.size() < source_int_vector.size()) {
		seed_vector = convert_int_to_int_vector(seed);
		digit_one = seed_vector[seed_vector.size() / 2];
		digit_two = seed_vector[seed_vector.size() / 2] + 1;
		new_iteration = digit_one * 10 + digit_two;
		new_seed = new_iteration * new_iteration * new_iteration;
		new_seed_vector = convert_int_to_int_vector(new_seed);
		for (int i : new_seed_vector) { shuffle_vector.push_back(i); }
	}
	while (shuffle_vector.size() > source_int_vector.size()) { shuffle_vector.pop_back(); }
	vector<int> source_placement_vector;
	for (int i = 0; i<source_int_vector.size(); i++) { source_placement_vector.push_back(i); }
	int switch_with_forward;
	int position_one;
	int position_two;
	for (int i = 0; i < shuffle_vector.size(); i++) {
		switch_with_forward = shuffle_vector[i];
		while (switch_with_forward > source_placement_vector.size() - 1) { switch_with_forward -= source_placement_vector.size(); }
		position_one = i;
		if (i + switch_with_forward > source_placement_vector.size() - 1) { position_two = i + switch_with_forward - source_placement_vector.size(); }
		else { position_two = i + switch_with_forward; }
		swap(source_placement_vector[position_one], source_placement_vector[position_two]);
	}
	vector<int> result_vector;
	for (int i = 0; i < source_int_vector.size(); i++) { result_vector.push_back(0); }
	for (int i = 0; i < source_placement_vector.size(); i++) { result_vector[source_placement_vector[i]] = source_int_vector[i]; }
	for (int i : result_vector) { result += to_string(i) + " "; }
	result.pop_back();
	return result;
}

string de_char_swap(string source, int iteration) {
	string result = "";
	vector<int> source_vector = convert_string_to_int_vector(source);
	int swap_vector_size = ((int)source_vector.size() / 2) * 10;
	vector<int> swap_vector = convert_int_seed_to_custom_length_vector(iteration, swap_vector_size);
	vector<int> to_erase;
	int removal_counter = 0;
	for (int i = 0; i < swap_vector.size(); i++) { if (swap_vector[i] >= source_vector.size()) { to_erase.push_back(i); } }
	for (int i : to_erase) { swap_vector.erase(swap_vector.begin() + (i - removal_counter)); removal_counter++; }
	vector<int> swap_vector_no_repeat;
	for (int i : swap_vector) { if (!vector_contains(swap_vector_no_repeat, i)) { swap_vector_no_repeat.push_back(i); } }
	swap_vector = swap_vector_no_repeat;
	if (swap_vector.size() % 2 != 0) { swap_vector.pop_back(); }
	int swap_from_position;
	int swap_to_position;
	int swap_value_from;
	int swap_value_to;
	for (int i = 0; i < swap_vector.size(); i += 2) {
		swap_from_position = swap_vector[i + 1];
		swap_to_position = swap_vector[i];
		swap_value_from = source_vector[swap_from_position];
		swap_value_to = source_vector[swap_to_position];
		source_vector[swap_from_position] = swap_value_to;
		source_vector[swap_to_position] = swap_value_from;
	}
	for (int i : source_vector) { result += to_string(i) + " "; }
	result.pop_back();
	return result;
}

string char_shift_right(string source, int iteration) {
	vector<int> intVector = convert_string_to_int_vector(source);
	vector<int> newVector;
	string result = "";
	while (iteration > intVector.size()) { iteration -= intVector.size(); }
	for (int i = 0; i < iteration; i++) { newVector.push_back(intVector.end()[i - iteration]); }
	for (int i = 0; i < intVector.size() - iteration; i++) { newVector.push_back(intVector[i]); }
	for (int i : newVector) { result += to_string(i) + " "; }
	result.pop_back();
	return result;
}

string char_shift_left(string source, int iteration) {
	vector<int> intVector = convert_string_to_int_vector(source);
	vector<int> newVector;
	string result = "";
	while (iteration > intVector.size()) { iteration -= intVector.size(); }
	for (int i = 0; i < intVector.size() - iteration; i++) { newVector.push_back(intVector.begin()[i + iteration]); }
	for (int i = 0; i < iteration; i++) { newVector.push_back(intVector[i]); }
	for (int i : newVector) { result += to_string(i) + " "; }
	result.pop_back();
	return result;
}

string de_char_shift(string source, int iteration) {
	string result;
	if ((int)iteration % 2 == 0) { result = char_shift_left(source, iteration); }
	else { result = char_shift_right(source, iteration); }
	return result;
}

string de_char_addition(string source, int iteration) {
	string result;
	vector<int> split_iteration_vector = convert_int_to_int_vector(iteration);
	int summation = 0;
	for (int i : split_iteration_vector) { summation -= i; }
	vector<int> split_source_vector = convert_string_to_int_vector(source);
	for (int i = 0; i < split_source_vector.size(); i++) { split_source_vector[i] += summation; }
	for (int i : split_source_vector) { result += to_string(i) + " "; }
	result.pop_back();
	return result;
}

// decryption control method
string decrypt(string source, string key) {
	string result = source;
	int key_int = convert_key_to_int(key);
	vector<int> iteration_numerals = convert_key_to_int_vector(key);
	vector<int> scramble_sequence = convert_int_vector_to_scramble_sequence(iteration_numerals, 4);
	reverse(scramble_sequence.begin(), scramble_sequence.end());
	reverse(iteration_numerals.begin(), iteration_numerals.end());
	for (int i = 0; i < scramble_sequence.size(); i++) {
		if (scramble_sequence[i] == 1) { result = de_char_shift(result, iteration_numerals[i]); }
		if (scramble_sequence[i] == 2) { result = de_char_shuffle(result, iteration_numerals[i]); }
		if (scramble_sequence[i] == 3) { result = de_char_swap(result, iteration_numerals[i]); }
		if (scramble_sequence[i] == 4) { result = de_char_addition(result, iteration_numerals[i]); }
		// to add another encryption method, follow the below example:
		// if (scramble_sequence[i] == *5*) { result = *decryption_method*(result, iteration_numbers[i]);
	}
	return result;
}

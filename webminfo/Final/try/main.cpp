#include <algorithm>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <bitset>
#include <vector>
#include <array>
#include <chrono>
#include <iomanip>

#include <unistd.h>
#include <time.h>

#define BUFSIZE 1048576 // 1MB

enum ebml_element_type {
	MASTER,
	UINT,
	INT,
	STRING,
	UTF8,
	BINARY,
	FLOAT,
	DATE
};

class simple_vint{
public:
	uint8_t width;
	uint8_t data[8];

	bool is_all_ones(){
		for(int i = 1; i < width - 1; i++){
			if(data[i] != 255){
				return false;
			}
		}
		return true;
	}

	uint64_t get_uint(){
		uint64_t value = 0;
		value = data[width - 1];
		for(int i = width - 1; i > 0; --i){
			value += ((uint64_t)data[i - 1] << ((width - i) * 8));
		}
		return value;
	}

	// Unused, good point though.
	uint64_t get_little_endian_uint(){
		uint64_t value = 0;
		value = data[0];
		for(int i = 0; i < width - 1; ++i){
			value += ((uint64_t)data[i + 1] << (i * 8));
		}
		return value;
	}
};

class ebml_element{
public:
	std::string name;
	std::array<uint8_t, 4> id;
	enum ebml_element_type type;

	ebml_element(std::string name, std::array<uint8_t, 4> const& id, enum ebml_element_type type)
	:name(name), id(id), type(type){}
};

const int SPEC_LEN = 250;

#include "spec.cpp"

ebml_element* get_element(std::array<uint8_t, 4> id, uint8_t level){
	bool found;
	for(int i = 0; i < SPEC_LEN; ++i){
		found = true;
		for(int j = 0; j < level; ++j){
			if(ebml_spec[i].id[j] != id[j]){
				found = false;
				break;
			}
		}
		if(found){
			return ebml_spec + i;
		}
	}
	return 0;
}

class ebml_parser{
public:
void parse(FILE* in, void(*callback)(const uint8_t* buffer, size_t len, const ebml_element* elem, const uint8_t* data)) {
	int len, mask, pos = 0;
	uint8_t buffer[BUFSIZE];
	uint8_t *carriage;
	uint8_t bits;

	while(1){
		carriage = buffer;
		// Get EBML Element ID first byte.
		if((len = fread(carriage, 1, 1, in)) < 0){
			std::cout << "Uh oh, read first id byte error!\n";
			break;
		}else if(len == 0){
			std::cout << "DONE!" << std::endl;
			break;
		}
		pos++;

		if(carriage[0] == 0){
			printf("encountered 0 byte, refuse to parse further\n");
			break;
		}

		bits = carriage[0];

		simple_vint id;
		id.width = 1;
		mask = 0x80;
		// Get EBML Element ID vint width.
		while(!(bits & mask)){
			mask >>= 1;
			id.width++;
		}

		id.data[0] = bits;
		carriage++;
		// Get EBML Element ID vint data.
		if((len = fread(carriage, 1, id.width - 1, in)) != id.width - 1){
			std::cout << "Uh oh, read id data error!\n";
			break;
		}
		pos += id.width - 1;
		// Get EBML Element ID.
		for(int i = 1; i < id.width; ++i){
			id.data[i] = carriage[i - 1];
		}

		carriage += id.width - 1;
		// Get EBML Element Size first byte.
		if((len = fread(carriage, 1, 1, in)) != 1){
			std::cout << "Uh oh, read first size byte error!\n";
			break;
		}
		pos++;

		bits = carriage[0];
		simple_vint size;
		size.width = 1;
		mask = 0x80;
		// Get EBML Element Size vint width.
		while(!(bits & mask)){
			mask >>= 1;
			size.width++;
		}

		bits ^= mask;
		size.data[0] = bits;
		carriage++;
		// Get EBML Element Size vint data.
		if((len = fread(carriage, 1, size.width - 1, in)) != size.width - 1){
			std::cout << "Uh oh, read id data error!\n";
			break;
		}
		pos += size.width - 1;

		// Get EBML Element Size.
		for(int i = 1; i < size.width; ++i){
			size.data[i] = carriage[i - 1];
		}

		// Specification for ID lookup.
		ebml_element* e = get_element(
			{{id.data[0], id.data[1], id.data[2], id.data[3]}},
			id.width);

		carriage += size.width - 1;
		if(e != 0){
			if(e->type != MASTER){
				// Get EBML Element Data, parse it.
				uint64_t data_len = size.get_uint();
				if((len = fread(carriage, 1, data_len, in) != data_len)){
					std::cout << "Uh oh, could not read all the data!" << std::endl;
					std::cout << "Wanted " << data_len << " found " << len << std::endl;
					break;
				}
				pos += data_len;
				std::cout << '(' << std::dec << pos << ") " << e->name;
				std::cout << std::endl;
				callback(buffer, carriage + data_len - buffer, e, carriage);
				carriage += data_len;
			}else{
				// Master data is actually just more elements, continue.
				std::cout << '(' << std::dec << pos << ')' << " ----- " << e->name << " [";
				if(size.is_all_ones()){
					std::cout << "Unknown";
				}else{
					std::cout << size.get_uint();
				}
				std::cout << ']' << std::endl;
				callback(buffer, carriage - buffer, e, nullptr);
			}
		}else{
			std::cout << "UNKNOWN ELEMENT!" << std::endl;
			break;
		}
	}
}
};

FILE* out;
uint8_t out_buffer[BUFSIZE];
size_t out_len = 0;
bool is_track_entry = false;
struct track_entry {
	int track_number;
	uint64_t track_type;
} last_track_entry{0, 0};
std::vector<int> audio_track_numbers;

uint64_t read_number(const uint8_t* data, int data_len)
{
	simple_vint track_number;
	track_number.width = 0;
	for(int i = 0; i < data_len; ++i){
		track_number.data[i] = data[i];
		track_number.width++;
	}
	return track_number.get_uint();
}

void flush_to_out_callback(const uint8_t* buffer, size_t len, const ebml_element* elem, const uint8_t* data)
{
	if (elem->name == "TrackEntry") {
		if (is_track_entry) { // new track entry right after previous one
			if (last_track_entry.track_type != 2) {
				if (fwrite(out_buffer, 1, out_len, out) != out_len) {
					printf("failed to write to output\n");
					return;
				}
			} else {
				audio_track_numbers.push_back(last_track_entry.track_number);
			}
		}
		is_track_entry = true;
		mempcpy(out_buffer, buffer, len);
		out_len = len;
		return;
	}
	if (elem->name == "Cues" 
	|| elem->name == "Void" 
	|| elem->name == "Cluster") {
		is_track_entry = false;
		if (last_track_entry.track_type != 2) {
			if (fwrite(out_buffer, 1, out_len, out) != out_len) {
				printf("failed to write to output\n");
				return;
			}
		} else {
			audio_track_numbers.push_back(last_track_entry.track_number);
		}
	}
	if (is_track_entry) {
		int data_len = buffer + len - data;
		if (elem->name == "TrackNumber") {
			last_track_entry.track_number = read_number(data, data_len);
		} else if (elem->name == "TrackType") {
			last_track_entry.track_type = read_number(data, data_len);
		}
		mempcpy(out_buffer + out_len, buffer, len);
		out_len += len;
		return;
	}

	if(elem->type == BINARY){
		if(elem->name == "SimpleBlock" || elem->name == "Block") {
			uint8_t bits = data[0];
			simple_vint track_number;
			track_number.width = 1;
			uint8_t mask = 0x80;
			while(!(bits & mask)){
				mask >>= 1;
				track_number.width++;
			}
			track_number.data[0] = data[0] ^ mask;
			for(int i = 1; i < track_number.width; ++i){
				track_number.data[i] = data[i];
			}
			uint64_t track_number_target = track_number.get_uint();
			std::cout << "Track Number: " << track_number_target << std::endl;
			bool is_audio = std::find(audio_track_numbers.begin(), audio_track_numbers.end(), track_number_target) != audio_track_numbers.end();
			if (is_audio)
				return;
		}
	}

	if (fwrite(buffer, 1, len, out) != len) {
		printf("failed to write to output\n");
		return;
	}
}

int main(int argc, char** argv){
	if (argc != 3) {
		printf("Usage: %s <input.webm> <output.webm>\n", argv[0]);
		return 1;
	}
	FILE* in = fopen(argv[1], "rb");
	if (!in){
		printf("cannot open input file\n");
		return 1;
	}
	out = fopen(argv[2], "wb");
	if (!out){
		printf("cannot open output file\n");
		fclose(in);
		return 2;
	}

	ebml_parser p;
	p.parse(in, &flush_to_out_callback);

	fclose(in);
	fclose(out);
	return 0;
}

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
void parse(FILE * in, FILE * out){
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
			std::cout << "Read '0' byte..." << std::endl;
			if((fwrite(buffer, 1, carriage - buffer, out) != 1)) {
				printf("Can't open output");
				return;
			}
			continue;
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
			size.data[i] = carriage[i - 1];		}

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
				std::cout << '(' << std::dec << pos << ") " << e->name << ": ";
				if(e->type == BINARY){
					// for(int i = 0; i < data_len; ++i){
					// 	// I'll only care about the first 32 binary bytes.
					// 	if(i == 32){
					// 		std::cout << "...";
					// 		break;
					// 	}
					// 	std::cout << std::hex << (int)carriage[i];
					// }
					std::cout << std::endl;
					if(e->name == "SimpleBlock" || e->name == "Block"){
						bits = carriage[0];
						simple_vint track_number;
						track_number.width = 1;
						mask = 0x80;
						while(!(bits & mask)){
							mask >>= 1;
							track_number.width++;
						}
						bits ^= mask;
						for(int i = 0; i < track_number.width; ++i){
							track_number.data[i] = carriage[i];
						}
						std::cout << "Track Number: " << std::dec << (int)track_number.get_uint() << std::endl;
						int16_t timecode = (int16_t)(((uint16_t)carriage[track_number.width] << 8) | carriage[track_number.width + 1]);
						std::cout << "Timecode: " << std::dec << (int)timecode << std::endl;
					}
				}else if(e->type == UINT){
					simple_vint data;
					data.width = 0;
					for(int i = 0; i < data_len; ++i){
						data.data[i] = carriage[i];
						data.width++;
					}
					uint64_t val = data.get_uint();
					std::cout << std::dec << val;
					std::cout << std::endl;
				}else if(e->type == INT){
					simple_vint data;
					data.width = 0;
					for(int i = 0; i < data_len; ++i){
						data.data[i] = carriage[i];
						data.width++;
					}
					std::cout << std::dec << int64_t(data.get_uint()) << std::endl;
				}else{
					for(int i = 0; i < data_len; ++i){
						std::cout << std::hex << (int)carriage[i];
					}
					std::cout << std::endl;
				}
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
			}
		}else{
			std::cout << "UNKNOWN ELEMENT!" << std::endl;
		}
		const int len = carriage - buffer;
		if(fwrite(buffer, 1, len, out) != len) {
			printf("Failed to write output\n");
			return;
		} 
	}
}
};



int main(int argc, char** argv){
	if(argc != 3) {
		printf("Usage: %s <input.webm> <output.webm>\n", argv[0]);
		return 1;
	}
	FILE * input = fopen(argv[1], "rb");
	if(!input) {
		printf("Can't open input file\n");
		return -1;
	}
	FILE * output = fopen(argv[2], "wb");
	if(!output) {
		printf("Can't open output file!");
		fclose(input);
		return -2;
	}
	ebml_parser p;
	p.parse(input, output);
	fclose(input);
	fclose(output);
	return 0;
}

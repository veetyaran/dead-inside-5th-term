#include <algorithm>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <array>
#include <iomanip>
#include <optional>
#include <string_view>

#define BUFSIZE 1048576 // 1MB

#define LOG_POSITIONS 0

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

	bool is_all_ones() const {
		for(int i = 1; i < width - 1; i++){
			if(data[i] != 255){
				return false;
			}
		}
		return true;
	}

	uint64_t get_uint() const {
		uint64_t value = 0;
		value = data[width - 1];
		for(int i = width - 1; i > 0; --i){
			value += ((uint64_t)data[i - 1] << ((width - i) * 8));
		}
		return value;
	}
};

struct ebml_element{
	std::string_view name;
	std::array<uint8_t, 4> id;
	enum ebml_element_type type;
};

#include "spec.h"

const int SPEC_LEN = sizeof(ebml_spec) / sizeof(ebml_spec[0]);

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

template <class Callback>
static void parse_ebml(FILE* in, Callback&& callback) {
	int len, mask;
	uint64_t pos = 0;
	uint8_t buffer[BUFSIZE];
	uint8_t *carriage;
	uint8_t bits;

	while(1){
		carriage = buffer;
		uint64_t current_length = 0;
		// Get EBML Element ID first byte.
		if((len = fread(carriage, 1, 1, in)) < 0){
			std::cout << "Uh oh, read first id byte error!\n";
			break;
		}else if(len == 0){
			std::cout << "DONE!" << std::endl;
			break;
		}
		current_length++;

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
		current_length += id.width - 1;
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
		current_length++;

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
		current_length += size.width - 1;

		// Get EBML Element Size.
		for(int i = 1; i < size.width; ++i){
			size.data[i] = carriage[i - 1];
		}

		// Specification for ID lookup.
		ebml_element* e = get_element(
			{{id.data[0], id.data[1], id.data[2], id.data[3]}},
			id.width);

		carriage += size.width - 1;
		if(e == 0){
			std::cout << "unknown element at pos = " << pos << std::endl;
			break;
		}
		if(e->type != MASTER){
			// Get EBML Element Data, parse it.
			uint64_t data_len = size.get_uint();
			if((len = fread(carriage, 1, data_len, in) != data_len)){
				std::cout << "Wanted " << data_len << " found " << len << std::endl;
				break;
			}
#if LOG_POSITIONS
			std::cout << '(' << std::dec << pos << ") " << e->name << std::endl;
#endif
			callback(buffer, carriage + data_len - buffer, *e, carriage, pos);
			current_length += data_len;
			carriage += data_len;
		}else{
			// Master data is actually just more elements, continue.
#if LOG_POSITIONS
			std::cout << '(' << std::dec << pos << ')' << " ----- " << e->name << " [";
			if(size.is_all_ones()){
				std::cout << "Unknown";
			}else{
				std::cout << size.get_uint();
			}
			std::cout << ']' << std::endl;
#endif
			callback(buffer, carriage - buffer, *e, &size, pos);
		}
		pos += current_length;
	}
}

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

struct CountSizes {
	void operator()(const uint8_t* buffer, int len, ebml_element elem, const void* data, uint64_t pos)
	{
		const auto elem_name = elem.name;
		if (elem.type == MASTER)
		{
			const auto data_size = static_cast<const simple_vint*>(data)->get_uint();
			if (elem_name == "Segment") {
				segment_size = data_size;
			} else if (elem_name == "Tracks") {
				tracks_size = data_size;
			} else if (elem_name == "TrackEntry") {
				auto& track = audio_tracks.emplace_back();
				track.size = len + data_size;
				track.data_size = data_size;
				track.pos = pos;
			}
			return;
		}
		const uint8_t* raw_data = static_cast<const uint8_t*>(data);
		const uint64_t data_len = buffer + len - raw_data;
		if (!audio_tracks.empty())
		{
			auto& track = audio_tracks.back();
			if (elem.name == "TrackNumber") {
				track.number = read_number(raw_data, data_len);
			} else if (elem.name == "TrackType") {
				const auto track_type = read_number(raw_data, data_len);
				if (track_type != 2) {
					if (track.type == 2)
						return;
					audio_tracks.pop_back();
					return;
				}
				track.type = track_type;
			}
		}
	}

	uint64_t segment_size = 0;
	uint64_t tracks_size = 0;
	struct TrackEntry {
		uint64_t pos = 0;
		uint64_t size = 0;
		uint64_t data_size = 0;
		uint64_t number = 0;
		uint64_t type = 0;

		friend std::ostream& operator<<(std::ostream& os, TrackEntry te) {
			return os
				<< "pos: " << te.pos << '\n'
				<< "size: " << te.size << '\n'
				<< "data_size: " << te.data_size << '\n'
				<< "number: " << te.number << '\n'
				<< "type: " << te.type << '\n';
		}
	};

	std::deque<TrackEntry> audio_tracks;
};

struct RewriteWithNewSizes {
	void operator()(uint8_t* buffer, int len, ebml_element elem, void* data, uint64_t pos)
	{
		auto& audio_tracks = sizes.audio_tracks;
		if (audio_tracks.size() > 1) {
			auto& audio_track = audio_tracks.front();
			if (audio_track.pos == pos) {
				std::cout << "removing audio_track:\n " << audio_track << '\n';
				buffer[0] = 0xEC; // void-element
				audio_tracks.pop_front();
			}
		}
		if (fwrite(buffer, 1, len, out) != len) {
			printf("failed to write to output\n");
			return;
		}
	}

	CountSizes sizes;
	FILE* out;
};

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
	FILE* out;
	out = fopen(argv[2], "wb");
	if (!out){
		printf("cannot open output file\n");
		fclose(in);
		return 2;
	}

	CountSizes cs;
	parse_ebml(in, cs);
	std::cout
		<< "cs.segment_size: " << cs.segment_size << '\n'
		<< "cs.tracks_size: " << cs.tracks_size << '\n';

	if (cs.audio_tracks.empty()) {
		std::cout << "no audio track was found, retreat\n";
		fclose(in);
		fclose(out);
		return 0;
	}

	std::cout << "found these audio tracks:\n";
	for (const auto& audio_track : cs.audio_tracks ) {
		std::cout << audio_track << '\n';
	}

	fseek(in, 0, SEEK_SET);

	RewriteWithNewSizes new_sizes{ cs, out };
	parse_ebml(in, new_sizes);

	fclose(in);
	fclose(out);
	return 0;
}

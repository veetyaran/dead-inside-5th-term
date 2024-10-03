#include <iostream>
#include <fstream>
#include <memory>
#include <string>

#include "mkvparser/mkvparser.h"
#include "mkvparser/mkvreader.h"

void get_video_dimensions(const std::string &file_path) {
    mkvparser::MkvReader reader;
    if (reader.Open(file_path.c_str())) {
        std::cerr << "Could not open file." << std::endl;
        return;
    }

    long long pos = 0;
    mkvparser::EBMLHeader ebml_header;
    if (ebml_header.Parse(&reader, pos) < 0) {
        std::cerr << "Could not parse EBML header." << std::endl;
        return;
    }

    mkvparser::Segment* segment;
    if (mkvparser::Segment::CreateInstance(&reader, pos, segment)) {
        std::cerr << "Could not create segment." << std::endl;
        return;
    }

    std::unique_ptr<mkvparser::Segment> segment_ptr(segment);
    if (segment->Load() < 0) {
        std::cerr << "Could not load segment." << std::endl;
        return;
    }

    const mkvparser::SegmentInfo* segment_info = segment->GetInfo();
    const mkvparser::Tracks* tracks = segment->GetTracks();

    for (unsigned long i = 0; i < tracks->GetTracksCount(); ++i) {
        const mkvparser::Track* track = tracks->GetTrackByIndex(i);
        if (track->GetType() == mkvparser::Track::kVideo) {
            const mkvparser::VideoTrack* video_track = static_cast<const mkvparser::VideoTrack*>(track);
            const long long width = video_track->GetWidth();
            const long long height = video_track->GetHeight();

            std::cout << "Width: " << width << " pixels" << std::endl;
            std::cout << "Height: " << height << " pixels" << std::endl;
            return;
        }
    }

    std::cerr << "No video track found." << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <file_path>" << std::endl;
        return 1;
    }

    std::string file_path = argv[1];
    get_video_dimensions(file_path);
    return 0;
}

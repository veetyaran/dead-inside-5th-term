#include <iostream>
#include <fstream>
#include <matroska/KaxSegment.h>
#include <matroska/KaxTracks.h>
#include <ebml/EbmlStream.h>
#include <ebml/EbmlElement.h>

void remove_audio_tracks(const std::string &input_filename, const std::string &output_filename) {
    std::ifstream input(input_filename, std::ios::binary);
    if (!input.is_open()) {
        throw std::runtime_error("Could not open input file");
    }

    std::ofstream output(output_filename, std::ios::binary);
    if (!output.is_open()) {
        throw std::runtime_error("Could not open output file");
    }

    EbmlStream input_stream(input);
    EbmlStream output_stream(output);

    // Create an element to hold the segment
    KaxSegment segment;
    segment.Read(input_stream, EbmlElement::ContextSegment);

    // Iterate through the tracks and remove audio tracks
    KaxTracks *tracks = static_cast<KaxTracks *>(segment.FindFirstElt(KaxTracks::ClassInfos));
    if (tracks) {
        for (unsigned int i = 0; i < tracks->ListSize(); ++i) {
            KaxTrackEntry &track = static_cast<KaxTrackEntry &>((*tracks)[i]);
            if (track.TrackType() == track_audio) {
                // Remove audio track
                tracks->RemoveElt(i);
                --i; // Adjust index after removal
            }
        }
    }

    // Render the segment to the output file
    segment.Render(output_stream);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>\n";
        return 1;
    }

    try {
        remove_audio_tracks(argv[1], argv[2]);
        std::cout << "Audio tracks removed successfully, output saved to " << argv[2] << "\n";
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
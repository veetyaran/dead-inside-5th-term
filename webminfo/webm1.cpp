#include <iostream>
#include <fstream>
#include <ebml/EbmlStream.h>
#include <ebml/EbmlHeader.h>
#include <matroska/KaxSegment.h>
#include <matroska/KaxTracks.h>
#include <matroska/KaxTrackEntryData.h>
#include <matroska/KaxVideoTrack.h>

using namespace std;
using namespace libebml;
using namespace libmatroska;

int main() {
    const char* filename = "example.webm";

    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Cannot open file " << filename << endl;
        return 1;
    }

    EbmlStream ebmlStream(file);
    EbmlElement* element = nullptr;
    EbmlHeader ebmlHeader;
    ebmlHeader.Read(ebmlStream, element);

    KaxSegment* segment = nullptr;
    bool done = false;

    while (!done && (element = ebmlStream.FindNextID(EbmlId(KaxSegment::ClassInfos), 0xFFFFFFFFL))) {
        segment = static_cast<KaxSegment*>(element);
        segment->Read(ebmlStream, element);

        KaxTracks* tracks = segment->FindChild<KaxTracks>();
        if (!tracks) continue;

        for (auto& trackEntry : tracks->GetElementList()) {
            KaxTrackEntry* track = static_cast<KaxTrackEntry*>(trackEntry);
            KaxVideoTrack* video = track->FindChild<KaxVideoTrack>();
            if (video) {
                for (auto& videoChild : video->GetElementList()) {
                    if (EbmlId(*videoChild) == EbmlId(KaxVideoPixelWidth::ClassInfos)) {
                        KaxVideoPixelWidth* pixelWidth = static_cast<KaxVideoPixelWidth*>(videoChild);
                        cout << "PixelWidth: " << uint64(*pixelWidth) << endl;
                        done = true;
                        break;
                    }
                }
            }
            if (done) break;
        }
    }

    if (!done) {
        cerr << "PixelWidth not found in the file." << endl;
    }

    return 0;
}
